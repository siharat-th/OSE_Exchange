#include "WavgCalcMulti.hpp"
#include <chrono>
#include <ctime>
#include <sstream>

WavgCalcMulti::WavgCalcMulti(TimerCallback & cb)
    : _cb(cb),_bActive(false),_bTimerTriggered(false)
{
}

void WavgCalcMulti::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void WavgCalcMulti::Init(string product, string startmonth, int startyear, int nummonths)
{
	MatrixGen mtx;
	mtx.GenOutrightMatrix(product, startmonth, startyear, nummonths);
	mtx.GenSpreadMatrix(product, startmonth, startyear, nummonths);

	m_GraphMap = mtx.GetGraphMap();

	_NumMonths = nummonths;

	int iStep = 1;

	_rows = 0;
	_cols = 0;
	_LegCount = 0;

	//here, we register the cme mkt data symbols. Iterate over the graph and go....
	//this RegisterLegByRowCol
	for (int n = 0; n < nummonths; n+=iStep)
	{
		_rows ++;
		RegisterLegByRowCol(n, n, KOrderProdType::FUTURE);

		for (int j = n+iStep; j < nummonths; j+=iStep)
		{
			_cols++;
			RegisterLegByRowCol(n, j, KOrderProdType::SPREAD);
		}
	}

	GenStprPairs();
}

void WavgCalcMulti::GenStprPairs()
{
	int n = 0;
	for (int j = 0; j < _NumMonths; j++)
	{
		n = j;
		std::pair<int,int> node = std::make_pair(n,j);

		//cout << "STPR FOR [" << n << "," << j << "]=" << endl;
		if (j == 0 && n == 0){

			stprpair x = {};
			x.add(node);
			//x.add(std::make_pair(_INFINITE_COL, _INFINITE_COL));
			_stpairs[n].push_back(x);

			//cout << "(0,0)" << endl;
			continue;
		}
		int cnt = 0;
		for (int row = 0; row < j; row++)
		{
			stprpair x = {};

			for(int col = 0; col <= j; col ++)
			{
				if (row > col)
					continue;

				if (row != col && col !=j)
					continue;

				std::pair<int,int>newnode = std::make_pair(row,col);
				x.add(newnode);

				if (x.cnt == 2)
					_stpairs[n].push_back(x);


				//cout << cnt << "=" << node_str(row,col) <<"; " << endl;
				cnt ++;
				//if (col == j)
					//cout << endl;
			}
		}
	}


	LogMe("----------- ST PAIRS CHECKSUM -----------",LogLevel::INFO);
	for(int i = 0; i < _NumMonths; i++)
	{
		ostringstream oss;
		oss << i << ":   PAIRS FOR [" << i << "," << i <<"]=";
		for(auto pr : _stpairs[i])
		{
			oss << "(" << pr.m1.first << "," << pr.m1.second << ")";
			oss << "(" << pr.m2.first << "," << pr.m2.second << ")";
			oss << " | ";
		}
		LogMe(oss.str());
	}
	LogMe("----------------------------------------",LogLevel::INFO);

}

void WavgCalcMulti::RegisterLegByRowCol(int n, int j, KOrderProdType::Enum prodtype)
{
	string symbol = m_GraphMap[std::make_pair(n,j)];
	//string prodtype = "spread";

	//instrument ln = LegParamParser::GetCmeLeg(symbol, prodtype, _cp.LegCount);

	//the index of where the algomd is returned to from mkt data server pointed to by coords
	_indices[n][j] = _LegCount;
	_mdimp[n][j] = {};

	_IndexToPathMap[_LegCount] = std::make_pair(n, j);

	_LegCount++;

	//test
	int index = _indices[n][j];
	string symb = m_GraphMap[std::make_pair(n,j)];

	ostringstream oss;
	oss << "WAVG ADDED LEG [" << n << "," << j <<"]: index=" << index << " SYMB=" << symb
			 << " INDEX=" << _indices[n][j] << " TOTAL=" << _LegCount;
	LogMe(oss.str(),LogLevel::INFO);
}

bool WavgCalcMulti::setStartEndTime(const std::string& startTime, const std::string& endTime) {
	StartTime = startTime;
	EndTime = endTime;

    // Get the current time
    auto currentTime = std::chrono::system_clock::now();
    auto currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    std::tm tmCurrentTime = *std::localtime(&currentTimeT);

    // Convert start and end times to std::tm structure for comparison
    std::tm tmStartTime = {};
    std::tm tmEndTime = {};


	if (strptime(startTime.c_str(), "%T", &tmStartTime) == nullptr || strptime(endTime.c_str(), "%T", &tmEndTime) == nullptr) {
		ostringstream oss;
		oss << "#1 Invalid time format. Expected format: HH:mm:ss";
		LogMe(oss.str(),LogLevel::ERROR);
		return false;
	}


	// Compare validity conditions
	if (std::difftime(std::mktime(&tmEndTime), std::mktime(&tmStartTime)) < 0) {
		ostringstream oss;
		oss << "#2 End time must be after start time.";
		LogMe(oss.str(),LogLevel::ERROR);
		return false;
	}

	if (std::difftime(std::mktime(&tmCurrentTime), std::mktime(&tmStartTime)) < 0) {
		ostringstream oss;
		oss << "#3 Start time must be after the current time." ;
		LogMe(oss.str(),LogLevel::ERROR);
		return false;
	}


	if (tmStartTime.tm_hour >= 17 || tmEndTime.tm_hour >= 17)
	{
		ostringstream oss;
		oss << "#5 Start time and end time must be before 17:00:00.";
		LogMe(oss.str(),LogLevel::ERROR);

		return false;
	}

	// All validity conditions are met
	LogMe("TIMES ARE VALID! STARTING THE WAVE. IF NOT RUNNING, WILL START THREAD", LogLevel::INFO);
	Start();
	return true;

}

void WavgCalcMulti::Test()
{
	if (_bTimerTriggered)
	{
		cout << "STOPPING WAVG!!!!" << endl;
		_bTimerTriggered = false;
	}

	LogMe( "STARTING WAVG TEST");

	int minutesToAdd = 2;
	std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string st = formatTime(currentTime);

	const std::time_t oneMinute = 60;
	std::time_t newTime = currentTime + (minutesToAdd * oneMinute);
	std::string et = formatTime(newTime);

	setStartEndTime(st, et);
	Start();


}

void WavgCalcMulti::Start() {
    if (Started())
    {
    	LogMe("TIMER THREAD ALREADY RUNNING!", LogLevel::ERROR);
    	return;
    }

    _bActive = true;
	LogMe("START CMD: CREATING WAVG TIMER THREAD", LogLevel::INFO);
	pthread_t tid;
	int rc = 0;
	if ((rc = pthread_create(&tid, NULL, timer_thread, this)) != 0)
	{
		LogMe("ERROR CREATING WAQVG TIMER THREAD", LogLevel::ERROR);
		std::exit(1);
	}

}

void WavgCalcMulti::Stop()
{
	LogMe("STOP CMD: STOPPING WAVG THREAD", LogLevel::WARN);
	_bActive = false;
}

void * WavgCalcMulti::timer_thread(void * arg)
{
	WavgCalcMulti * a = (WavgCalcMulti*)arg;

	cpu_set_t cpus;
	CPU_ZERO(&cpus);
	CPU_SET(1, &cpus);

	pthread_t current_thread = pthread_self();
	pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);

	std::string me =  "wavg-calc";

	int irc  = pthread_setname_np(current_thread, me.c_str());
	if (irc != 0)
	{
		a->LogMe("ERROR NAMING THREAD", LogLevel::ERROR);
		exit(1);
	}

	a->TimerLoop();

	a->LogMe("EXITING TIMER THREAD!", LogLevel::WARN);

	return NULL;
}

void WavgCalcMulti::TimerLoop()
{
	LogMe("STARTING WAVG THREAD CALCULATION. WAITING FOR START: " + StartTime, LogLevel::WARN);

	while (getCurrentTime() != StartTime) {
		usleep(1);
		if (!_bActive)
		{
			_bTimerTriggered = false;
			return;
		}
	}

	LogMe("!!! BANG !!! TIMER STARTED!!!", LogLevel::INFO);

	_waves.clear();
//	weightedAverages.clear();
//	lastWeightedAverage = 0.0;
//	totalWeightedSum = 0.0;
//	totalSize = 0.0;
//	count = 0;

	_bTimerTriggered = true;
	_cb.onTimerStateChange(TimerState::START);

	while (getCurrentTime() != EndTime) {

		usleep(1);
		if (!_bActive)
		{
			_bTimerTriggered = false;
			return;
		}
	}

	_bTimerTriggered = false;
	_bActive = false;

	_cb.onTimerStateChange(TimerState::END);

	LogMe("END TIME!!!!!!!!!!!", LogLevel::INFO);

	for(auto kv : _waves){
		ostringstream oss;
		oss << fixed << std::setprecision(0) << "INDEX (" << kv.first.first << "," <<
				kv.first.second << ") FINAL WAVG=" << kv.second.wavg
				<< " COUNT=" << kv.second.count << " VOL="<< kv.second.volume;
		LogMe(oss.str());
	}

}


void WavgCalcMulti::calculate(int n, int j, int64_t price, int32_t size) {
    if (!_bTimerTriggered) {
        return;
    }

    std::pair<int,int> node = std::make_pair(n,j);

	_waves[node].wsum += (double)price * (double)size;
	_waves[node].volume += (double)size;

	_waves[node].wavg = _waves[node].wsum / _waves[node].volume;

	_waves[node].count ++;

//	if (node.first == 0 && node.second == 0)
//	{
//		cout << "(" << n << "," << j <<"): px=" << price << " sz=" << size << " cnt=" << _waves[node].count
//				<< " vol=" << _waves[node].volume << " wsum=" << _waves[node].wsum
//				<< " wavg=" << _waves[node].wavg << endl;
//	}

	CalculateSettlements();

}

void WavgCalcMulti::CalculateSettlements()
{

	for(int i = 0; i < _NumMonths; i++)
	{
		std::pair<int,int> node = std::make_pair(i,i);

		//cout <<"**** NODE " << node.first << "," << node.second << " ******" << endl;

		if (i == 0)
		{
			_waves[node].stpr = _waves[_stpairs[i][0].m1].wavg;
			//cout << "STPR " << node.first << "," << node.second << "=" << _waves[node].stpr << endl << endl;
			continue;
		}
		if (i == 1)
		{
			_waves[node].stpr = _waves[_stpairs[i][0].m1].stpr - _waves[_stpairs[i][0].m2].wavg;
		//	cout << "STPR " << node.first << "," << node.second << "=" << _waves[node].stpr << endl << endl;
			continue;
		}

		double stprtemp = 0;
		for(auto pr : _stpairs[i])
		{
			//each pr is a pair of 2 months that we have to weight etc.
			double wf = WF(pr.m2.first, pr.m2.second);
			double temp = (_waves[pr.m1].stpr - _waves[pr.m2].wavg) * wf;
			stprtemp += temp;


//			cout    << "  ("
//					<<  pr.m1.first << "," << pr.m1.second
//					<< ") - ("
//					<<  pr.m2.first << "," << pr.m2.second
//					<< "):  wf=" << wf << " stpr1=" << _waves[pr.m1].stpr
//					<< " wavg2=" <<   _waves[pr.m2].wavg << endl;
		}
		_waves[node].stpr = stprtemp;
		//cout << "STPR " << node.first << "," << node.second << "=" << _waves[node].stpr << endl << endl;
	}


	for(int row = 0; row < _NumMonths; row ++)
	{
		for(int col = row + 1; col < _NumMonths; col++)
		{
			_waves[std::make_pair(row,col)].stpr = _waves[std::make_pair(row,row)].stpr
					- _waves[std::make_pair(col,col)].stpr;

//			cout << pn(row,col) << " st=" << _waves[std::make_pair(row,col)].stpr
//					<< " " << pn(row,row) << "=" << _waves[std::make_pair(row,row)].stpr
//					<< " " << pn(col,col) << "=" << _waves[std::make_pair(col,col)].stpr
//					<< endl;
		}
	}

//	cout << "******* DBG **********" << endl;
//	for(int i = 0; i < _NumMonths; i++)
//	{
//		std::pair<int,int> node = std::make_pair(i,i);
//		cout << "STPR " << node.first << "," << node.second << "=" << _waves[node].stpr << endl;
//	}
}

//double WavgCalcMulti::CalcStpr(int n)
//{
//	if (n == 0)
//	{
//		 _waves[std::make_pair(n, n)].stpr =  _waves[std::make_pair(n, n)].wavg;
//		return _waves[std::make_pair(n, n)].stpr;
//	}
//
//	for(auto pr : _stpairs[n])
//	{
//		double w1 = _waves[pr.m1]
//	}
//
//
//}

//Total volume across j cols
double WavgCalcMulti::TV(int j)
{
	double volume = 0;
	for(int i = 0; i < j; i++)
	{
		double denom = (double)MDist(i,j);

		std::pair<int,int> node = std::make_pair(i,j);
		volume += (static_cast<double>(_waves[node].volume) / denom);
	}

	return volume;
}

//Weight Factor
double WavgCalcMulti::WF(int n, int j)
{
	int volume = 0;

	int m = MDist(n,j);

	double tv = TV(j);

	volume = _waves[std::make_pair(n,j)].volume;

	double res = (static_cast<double>(volume) / m) / tv;

//	cout << "WF(" << n << "," << j << "): v=" << volume << " m=" << m << " tv=" << tv
//			<< " res=" << res << endl;

	return res;

}

double WavgCalcMulti::getStpr(int n, int j)
{
//	if (n == j)
//		return _waves[std::make_pair(n,j)].stpr;

	double res = _waves[std::make_pair(n,j)].stpr;// - _waves[std::make_pair(j,j)].stpr;
	return res;
}

double WavgCalcMulti::getWavg(int n, int j)
{
	double res = _waves[std::make_pair(n,j)].wavg;
	return res;
}

double WavgCalcMulti::getVolume(int n, int j)
{
	return _waves[std::make_pair(n,j)].volume;
}

double WavgCalcMulti::getCount(int n, int j)
{
	return _waves[std::make_pair(n,j)].count;
}

bool WavgCalcMulti::Triggered()
{
	return _bTimerTriggered;
}

bool WavgCalcMulti::Started()
{
	return _bActive;
}

std::string WavgCalcMulti::getCurrentTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return formatTime(currentTime);
}

std::string WavgCalcMulti::formatTime(std::time_t currentTime) {
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}


