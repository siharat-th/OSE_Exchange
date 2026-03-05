#include "SimpleTimer.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <thread>

using namespace KTN::ALGO;


SimpleTimer::SimpleTimer(TimerCallback & cb)
    : _cb(cb), _LAST_SENT(0)
{
	_TRIGGERED.store(false, std::memory_order_relaxed);
	_ACTIVE.store(false, std::memory_order_relaxed);
}

SimpleTimer::~SimpleTimer()
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void SimpleTimer::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void SimpleTimer::Start(const std::string& startTime, const std::string& endTime)
{
    if (!Validate(startTime, endTime))
    {
    	return;
    }

    _PAUSED.store(false, std::memory_order_relaxed);

	_StartTime = startTime;
	_EndTime = endTime;

	if (Started())
	{
		LogMe("INTERVAL TIMER THREAD ALREADY RUNNING- PARAMS UPDATED!", LogLevel::WARN);
		return;
	}
	//NamedThread impl
	CreateAndRun("simp-timer",1);
}

void SimpleTimer::Pause(bool paused)
{
	_PAUSED.store(paused, std::memory_order_relaxed);
	string tf = (_PAUSED.load(std::memory_order_relaxed)== true) ? "TRUE" : "FALSE";
	LogMe("PAUSE TIMER: PAUSE=" + tf,LogLevel::WARN);
}

void SimpleTimer::Stop()
{
	LogMe("STOP CMD: STOPPING TIMER THREAD", LogLevel::WARN);
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void SimpleTimer::Run()
{
	_TRIGGERED.store(false,std::memory_order_relaxed);
	_ACTIVE.store(true, std::memory_order_relaxed);
	_EXPIRED.store(false, std::memory_order_relaxed);

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
	uint64_t now;//, elapsed work;
	//uint64_t last_sent_time = 0;
	//TimerInterval::Enum currentFlag = TimerInterval::CALCULATING;
	int usleep_interval = 1000;


	auto endMicroseconds = MicrosecondsSinceEpoch(_EndTime);

	uint64_t endMicrosecondsCount = endMicroseconds.count();


	while(_ACTIVE.load(std::memory_order_acquire) == true)
	{
		//Here, we can just pause execution without exiting the thread....
		if (_PAUSED.load(std::memory_order_acquire) == true)
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
			_EXPIRED.store(true, std::memory_order_relaxed);
			continue;
		}

		//elapsed = now - last_sent_time;



		usleep(usleep_interval);
	}

	LogMe("INTERVAL TIMER END TIME!!!!!!!!!!!", LogLevel::INFO);
}

std::string SimpleTimer::getCurrentTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return formatTime(currentTime);
}

std::string SimpleTimer::formatTime(std::time_t currentTime) {
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}

bool SimpleTimer::Validate(const std::string& startTime, const std::string& endTime)
{
  // Get the current time
	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
	std::tm tmCurrentTime = *std::localtime(&currentTimeT);

	// Convert start and end times to std::tm structure for comparison
	std::tm tmStartTime = {};
	std::tm tmEndTime = {};


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
	return true;
}

// Function to convert local time string to UTC time in microseconds since epoch
std::chrono::microseconds SimpleTimer::MicrosecondsSinceEpoch(const std::string& timeStr) {
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

void SimpleTimer::waitForStart(const std::string& startTime, const std::string& endTime) {
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



