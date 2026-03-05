#include "IntervalTimer.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <thread>

using namespace KTN::ALGO;


IntervalTimer::IntervalTimer(TimerCallback & cb)
    : _cb(cb),_TimerMaxWorkMS(0),_LAST_SENT(0)
{
	_TRIGGERED.store(false, std::memory_order_relaxed);
	_ACTIVE.store(false, std::memory_order_relaxed);
}

IntervalTimer::~IntervalTimer()
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void IntervalTimer::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void IntervalTimer::Start(const std::string& startTime, const std::string& endTime,
		int intervalsecs, int maxworksecs)
{
    int intervalms = intervalsecs * 1000;
    int maxworkms = maxworksecs * 1000;

    if (!Validate(startTime, endTime, intervalms, maxworkms))
    {
    	return;
    }

    _PAUSED.store(false, std::memory_order_relaxed);

	_StartTime = startTime;
	_EndTime = endTime;
	//_TimerIntervalMS = intervalms;
	_TimerMaxWorkMS = maxworkms;

	UpdateInterval(intervalsecs);


	if (Started())
	{
		LogMe("INTERVAL TIMER THREAD ALREADY RUNNING- PARAMS UPDATED!", LogLevel::WARN);
		return;
	}
	//NamedThread impl
	CreateAndRun("simp-timer",1);
}

void IntervalTimer::UpdateInterval(int intervalsecs)
{
	int interval_millis = intervalsecs * 1000;
	uint64_t interval_us = static_cast<uint64_t>(interval_millis) * 1000ULL ;
	_TimerIntervalUS.store(interval_us, std::memory_order_relaxed);
	LogMe("TIMER INTERVAL UPDATED TO " + std::to_string(intervalsecs) + " SECS",LogLevel::INFO);
}

void IntervalTimer::Pause(bool paused)
{
	_PAUSED.store(paused, std::memory_order_relaxed);
	string tf = (_PAUSED.load(std::memory_order_relaxed)== true) ? "TRUE" : "FALSE";
	LogMe("PAUSE TIMER: PAUSE=" + tf,LogLevel::WARN);
}

void IntervalTimer::Stop()
{
	LogMe("STOP CMD: STOPPING TIMER THREAD", LogLevel::WARN);
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void IntervalTimer::Run()
{
	_TRIGGERED.store(false,std::memory_order_relaxed);
	_ACTIVE.store(true, std::memory_order_relaxed);

	for(int i = 0; i < 5; i++)
		LogMe("****STARTING TIMER THREAD. WAITING FOR START TIME: " + _StartTime, LogLevel::WARN);

	_cb.onTimerStateChange(TimerState::WAIT);

	waitForStart(_StartTime, _EndTime);

	//Now, if we come back here and not Triggered- we return; end it
	//So if we call stop-> ACTIVE=false -> waitForStart will exit -> Triggered = false -> return;
	if (_TRIGGERED.load(std::memory_order_relaxed) == false)
	{
		_cb.onTimerStateChange(TimerState::ERROR);
		LogMe("START TIME WAIT HAS RETURNED AND TRIGGER=False. EXITING!",LogLevel::ERROR);
		return;
	}

	LogMe("!!! BANG !!! TIMER STARTED!!!", LogLevel::INFO);
	_cb.onTimerStateChange(TimerState::START);

	_LAST_SENT = 0;
	uint64_t now, elapsed, work;
	uint64_t last_sent_time = 0;
	TimerInterval::Enum currentFlag = TimerInterval::CALCULATING;
	int usleep_interval = 1000;

	//how do we update these if needed?
//	int interval_millis = _TimerIntervalMS.load(std::memory_order_relaxed);
//	uint64_t interval_us = static_cast<uint64_t>(interval_millis) * 1000ULL ;

	int maxwork_millis = _TimerMaxWorkMS;
	uint64_t maxwork_us = static_cast<uint64_t>(maxwork_millis) * 1000ULL ;


	auto endMicroseconds = MicrosecondsSinceEpoch(_EndTime);

	uint64_t endMicrosecondsCount = endMicroseconds.count();


	while(_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		//Here, we can just pause execution without exiting the thread....
		if (_PAUSED.load(std::memory_order_relaxed) == true)
		{
			usleep(usleep_interval);
			continue;
		}

		//next, the guts... Sony guts.
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		now =  ((ts.tv_sec) * 1000000ULL) + ((ts.tv_nsec) / 1000ULL);

		if (now >= endMicrosecondsCount)
		{
			LogMe("TIMER EXPIRED!!!", LogLevel::WARN);
			_cb.onTimerStateChange(TimerState::END);
			_ACTIVE.store(false, std::memory_order_relaxed);
			continue;
		}

		elapsed = now - last_sent_time;

		if (_RESET_INTERVAL.load(std::memory_order_relaxed) == true)
		{
			LogMe("INTERVAL RESET!",LogLevel::INFO);
			_RESET_INTERVAL.store(false, std::memory_order_acquire);
			last_sent_time = now;
			_LAST_SENT = now;
			elapsed = 0;
		}

		if (currentFlag == TimerInterval::CALCULATING &&
				elapsed >= _TimerIntervalUS.load(std::memory_order_relaxed)) {
			currentFlag = TimerInterval::TRIGGERED;
			_cb.onTimerInterval(currentFlag);
			elapsed = 0;
			work = 0;
			last_sent_time = now;
			_LAST_SENT = now;
		}

		if (currentFlag == TimerInterval::TRIGGERED )
		{
			work = now - last_sent_time;
			//cout << "WORK=" << work << " now=" << now << " last=" << last_sent_time << " max=" << maxwork_us << endl;
			if (maxwork_us == 0 || work >= maxwork_us)
			{
				currentFlag = TimerInterval::CALCULATING;
				_cb.onTimerInterval(TimerInterval::MAXSECS);
				work = 0;
			}
		}

		usleep(usleep_interval);
	}

	LogMe("INTERVAL TIMER END TIME!!!!!!!!!!!", LogLevel::INFO);
}

std::string IntervalTimer::getCurrentTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return formatTime(currentTime);
}

std::string IntervalTimer::formatTime(std::time_t currentTime) {
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}

bool IntervalTimer::Validate(const std::string& startTime, const std::string& endTime,
		int intervalms, int maxworkms)
{
  // Get the current time
	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
	std::tm tmCurrentTime = *std::localtime(&currentTimeT);

	// Convert start and end times to std::tm structure for comparison
	std::tm tmStartTime = {};
	std::tm tmEndTime = {};

    if (intervalms <= 0)
    {
    	LogMe("INVALID TIMER INTERVAL. MUST BE > 0", LogLevel::ERROR);
    	return false;
    }

    if (maxworkms > 0 && maxworkms >= (intervalms-100))
    {
    	LogMe("INVALID MAX WORK INTERVAL. MUST BE AT LEAST 100ms LESS THAN INTERVAL",LogLevel::ERROR);
    	return false;
    }


	if (strptime(startTime.c_str(), "%T", &tmStartTime) == nullptr || strptime(endTime.c_str(), "%T", &tmEndTime) == nullptr) {
		ostringstream oss;
		oss << "#1 Invalid time format. Expected format: HH:mm:ss. Received:" << startTime;
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


//	if (tmStartTime.tm_hour >= 17 || tmEndTime.tm_hour >= 17)
//	{
//		ostringstream oss;
//		oss << "#5 Start time and end time must be before 17:00:00.";
//		LogMe(oss.str(),LogLevel::ERROR);
//		return false;
//	}

	return true;
}

// Function to convert local time string to UTC time in microseconds since epoch
std::chrono::microseconds IntervalTimer::MicrosecondsSinceEpoch(const std::string& timeStr) {
	// Get current date
	std::time_t now = std::time(nullptr);
	std::tm tm = *std::localtime(&now);

	// Parse the time string
	std::istringstream ss(timeStr);
	ss >> std::get_time(&tm, "%H:%M:%S"); // Only time, no date

	// Convert to time_t and then to microseconds since epoch
	std::time_t time_t_val = std::mktime(&tm);
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(time_t_val));

}

void IntervalTimer::waitForStart(const std::string& startTime, const std::string& endTime) {
    auto startMicroseconds = MicrosecondsSinceEpoch(startTime);
    auto endMicroseconds = MicrosecondsSinceEpoch(endTime);

    _TRIGGERED.store(false, std::memory_order_relaxed);

    while (_ACTIVE.load(std::memory_order_relaxed) == true)
    {
        auto now = std::chrono::system_clock::now();
        auto nowMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

        if (nowMicroseconds.count() >= startMicroseconds.count()) {
            LogMe("TIMER START TIME TRIGGERED!", LogLevel::INFO);
            _TRIGGERED.store(true, std::memory_order_relaxed);
            break;
        }

        if (nowMicroseconds.count() > endMicroseconds.count()) {
            LogMe("ERROR: END TIME IS BEFORE START!!!!", LogLevel::ERROR);
            break;
        }

        usleep(1000);
    }

    LogMe("RETURNING FROM WAIT FOR START");
}

void IntervalTimer::Test()
{
	bool trig = _TRIGGERED.load(std::memory_order_relaxed);
	if (trig)
	{
		cout << "STOPPING SIMPLE TIMER THREAD TEST!!!!" << endl;
		_TRIGGERED.store(false, std::memory_order_relaxed);
	}

	LogMe( "STARTING WAVG TEST");

	int minutesToAdd = 2;
	std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string st = formatTime(currentTime);

	const std::time_t oneMinute = 60;
	std::time_t newTime = currentTime + (minutesToAdd * oneMinute);
	std::string et = formatTime(newTime);

	Start(st, et, 10000, 5000);
}


