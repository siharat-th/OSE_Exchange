#include "WeightedAverageCalculator.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <thread>

WeightedAverageCalculator::WeightedAverageCalculator(TimerCallback & cb)
    : _cb(cb),_bActive(false),_bTimerTriggered(false)
{
}

void WeightedAverageCalculator::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

bool WeightedAverageCalculator::setStartEndTime(const std::string& startTime, const std::string& endTime) {

	StartTime =  parseTimeString(startTime);
	EndTime =  parseTimeString(endTime);

	_sttime = startTime;
	_endtime = endTime;


	//old chat gpt sucks ass stuff:

//	StartTime = startTime;
//	EndTime = endTime;



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

void WeightedAverageCalculator::Test()
{
	if (_bTimerTriggered)
	{
		cout << "STOPPING WAVG!!!!" << endl;
		_bTimerTriggered = false;
	}

	LogMe( "STARTING WAVG TEST", LogLevel::Enum::INFO);

	int minutesToAdd = 2;
	std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string st = formatTime(currentTime);

	const std::time_t oneMinute = 60;
	std::time_t newTime = currentTime + (minutesToAdd * oneMinute);
	std::string et = formatTime(newTime);

	setStartEndTime(st, et);
	Start();


}

void WeightedAverageCalculator::Start() {
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

void WeightedAverageCalculator::Stop()
{
	LogMe("STOP CMD: STOPPING WAVG THREAD", LogLevel::WARN);
	_bActive = false;
}

void * WeightedAverageCalculator::timer_thread(void * arg)
{
	WeightedAverageCalculator * a = (WeightedAverageCalculator*)arg;

	cpu_set_t cpus;
	CPU_ZERO(&cpus);
	CPU_SET(1, &cpus);

	pthread_t current_thread = pthread_self();
	pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);

	std::string me =  "wavg-calc";

	int irc  = pthread_setname_np(current_thread, me.c_str());
	if (irc != 0)
	{
		string err = "ERROR NAMING THREAD: " + me;
		//perror(err.c_str());
		cout << err << endl;
		exit(1);
	}

	a->TimerLoop();

	a->LogMe("EXITING TIMER THREAD!", LogLevel::WARN);

	return NULL;
}

void WeightedAverageCalculator::TimerLoop()
{
	LogMe("STARTING WAVG THREAD CALCULATION. WAITING FOR STARTTIME: " + _sttime, LogLevel::WARN);

	_bTimerTriggered = false;
	_cb.onTimerStateChange(TimerState::END);

	_waves.clear();

//	while (getCurrentTime() != StartTime) {
//		usleep(1);
//		if (!_bActive)
//		{
//			_bTimerTriggered = false;
//			return;
//		}
//	}

	while (!hasCurrentTimeExceededChatGPTSucks(StartTime)) {
	    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	    if (!_bActive) {
	        _bTimerTriggered = false;
	        return;
	    }
	}

	LogMe("!!! BANG !!! TIMER STARTED!!!", LogLevel::INFO);

	_bTimerTriggered = true;
	_cb.onTimerStateChange(TimerState::START);

	while (!hasCurrentTimeExceededChatGPTSucks(EndTime)) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		if (!_bActive) {
			_bTimerTriggered = false;
			return;
		}
	}

//	while (getCurrentTime() != EndTime) {
//
//		usleep(1);
//		if (!_bActive)
//		{
//			_bTimerTriggered = false;
//			return;
//		}
//	}

	_bTimerTriggered = false;
	_bActive = false;

	_cb.onTimerStateChange(TimerState::END);

	LogMe("END TIME!!!!!!!!!!!", LogLevel::INFO);

	for(auto kv : _waves){
		ostringstream oss;
		oss << fixed << std::setprecision(0) << "INDEX " << kv.first << " FINAL WAVG=" << kv.second.wavg
				<< " COUNT=" << kv.second.count << " VOL="<< kv.second.volume;
		LogMe(oss.str(), LogLevel::Enum::INFO);
	}

}


void WeightedAverageCalculator::calculate(int index, int64_t price, int32_t size) {
    if (!_bTimerTriggered) {
        return;
    }

    //while (Enabled) {
	_waves[index].wsum += (double)price * (double)size;
	_waves[index].volume += (double)size;

	// Update the last weighted average
	_waves[index].wavg = _waves[index].wsum / _waves[index].volume;

	// Store the weighted average in the map
	//std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//std::string timeStr = formatTime(currentTime);
//	weightedAverages[timeStr] = lastWeightedAverage;

	_waves[index].count ++;

//#if defined(TAS_TEST)
//	ostringstream oss;
//	oss << fixed << std::setprecision(4) <<  count << ": " << timeStr << " WAVG=" << lastWeightedAverage << " LASTPX=" << price << " LASTSZ=" << size << " VOL="<< totalSize;
//	LogMe(oss.str());
//#endif


        // Sleep for a certain period before the next calculation
   //     std::this_thread::sleep_for(std::chrono::seconds(1));
   // }

}

double WeightedAverageCalculator::getWavg(int index)
{
	double res = _waves[index].wavg;
	return res;
}

double WeightedAverageCalculator::getVolume(int index)
{
	return _waves[index].volume;
}

double WeightedAverageCalculator::getCount(int index)
{
	return _waves[index].count;
}

bool WeightedAverageCalculator::Triggered()
{
	return _bTimerTriggered;
}

bool WeightedAverageCalculator::Started()
{
	return _bActive;
}

std::string WeightedAverageCalculator::getCurrentTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return formatTime(currentTime);
}

std::string WeightedAverageCalculator::formatTime(std::time_t currentTime) {
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}


bool WeightedAverageCalculator::hasCurrentTimeExceededChatGPTSucks(const std::chrono::high_resolution_clock::time_point& endTime) {
    return std::chrono::high_resolution_clock::now() >= endTime;
}


