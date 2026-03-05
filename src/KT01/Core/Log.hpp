#pragma once

#if defined(_ICC) || defined(__INTEL_COMPILER)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <KT01/Core/Singleton.hpp>
#include <Loggers/LogEnums.hpp>
#include <KT01/Core/Settings.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>


#if defined(_ICC) || defined(__INTEL_COMPILER)
#else
#pragma GCC diagnostic pop
#endif

//COMMENT THIS OUT TO DELETE
//#define KT01_DEBUG_MESSAGING

#ifdef KT01_DEBUG_MESSAGING
#   ifndef SPDLOG_TRACE_ON
#       define SPDLOG_TRACE_ON 1
#   endif
#   ifndef SPDLOG_DEBUG_ON
#       define SPDLOG_DEBUG_ON 1
#   endif
#endif

template <typename T>
inline const T &format_if_needed(const T &value)
{
	return value;
}

template<typename FMT, typename... Args>
inline std::string format_if_needed(const FMT &fmt, const Args &... args)
{
	return fmt::format(fmt, args...);
} 

#define KT01_LOG_TRACE(label, ...)    SPDLOG_TRACE(KT01::Core::Log::instance().logger(), "[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_DEBUG(label, ...)    SPDLOG_DEBUG(KT01::Core::Log::instance().logger(), "[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_INFO(label, ...)     KT01::Core::Log::instance().logger()->info("[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_NOTICE(label, ...)   KT01::Core::Log::instance().logger()->notice("[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_WARN(label, ...)     KT01::Core::Log::instance().logger()->warn("[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_ERROR(label, ...)    KT01::Core::Log::instance().logger()->error("[{}] {}", label, format_if_needed(__VA_ARGS__))
#define KT01_LOG_CRITICAL(label, ...) KT01::Core::Log::instance().logger()->critical("[{}] {}", label, format_if_needed(__VA_ARGS__))
//color based
#define KT01_LOG_RED(label, ...)        KT01::Core::Log::instance().logger()->info("\033[31m[{}] {}\033[0m", label, fmt::format(__VA_ARGS__))
#define KT01_LOG_OK(label, ...)         KT01::Core::Log::instance().logger()->info("\033[32m[{}] {}\033[0m", label, fmt::format(__VA_ARGS__))
#define KT01_LOG_HIGHLIGHT(label, ...)  KT01::Core::Log::instance().logger()->warn("\033[33m[{}] {}\033[0m", label, fmt::format(__VA_ARGS__))
#define KT01_LOG_BLUE(label, ...)       KT01::Core::Log::instance().logger()->info("\033[34m[{}] {}\033[0m", label, fmt::format(__VA_ARGS__))

// Legacy color constants (used by EQT mktdata)
#define LOG_WHITE  7
#define LOG_RED    1
#define LOG_GREEN  2
#define LOG_YELLOW 3
#define LOG_CYAN   6

// Legacy AKL_LOG macro (color arg ignored — spdlog handles colors)
#define AKL_LOG(msg, label, color) KT01::Core::Log::instance().logger()->info("[{}] {}", label, msg)

#define LOGINFO(...)     KT01::Core::Log::instance().logger()->info(__VA_ARGS__)
#define LOGWARN(...)     KT01::Core::Log::instance().logger()->warn(__VA_ARGS__)
#define LOGERROR(...)     KT01::Core::Log::instance().logger()->error(__VA_ARGS__)

namespace KT01 { namespace Core {

class Log : public Singleton<Log> {
public:
	Log()
	{
		Settings settings("Settings.txt");
		std::string loggerType;
		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.Type", getString, loggerType, std::string("Console"));

		string pattern = "%H:%M:%S.%f [%l] %v";
		if (loggerType == "Console") {
			m_logger_sp = spdlog::stdout_color_mt("console", spdlog::color_mode::always);
			m_logger_sp->set_pattern(pattern);
		}
		else if (loggerType == "AsyncPool") {
			// Get the settings for the async logger
			int queueSize, threadCount;
			LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolQueueSize", getInteger, queueSize, 8192);
			LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolThreadCount", getInteger, threadCount, 1);
			std::vector<std::string> sinks;
			LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolSinks", getListCsv, sinks, std::vector<std::string>{});

			std::string dir, file;
			if (std::find(sinks.begin(), sinks.end(), "File") != sinks.end()) {
				LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolDir", getString, dir, std::string("Logs"));
				LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolFile", getString, file, std::string("akl-cme.log"));
			}

			std::string sinkThreadingType;
			LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Logger.AsyncPoolSinkThreadingType", getString, sinkThreadingType, std::string("mt"));

			// Create the sinks based on the settings
			std::vector<spdlog::sink_ptr> spd_sinks;
			for (auto sink : sinks) {
				if (sink == "Console") {
					if (threadCount == 1 && (sinkThreadingType == "st" || sinkThreadingType == "ST")) {
						auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
						console_sink->set_pattern("%H:%M:%S.%f %^[%l]%$ %v");
						spd_sinks.push_back(console_sink);

						std::cout << "Logger adding console sink with single-threaded mode" << std::endl;
					}
					else {
						auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
						console_sink->set_pattern("%H:%M:%S.%f %^[%l]%$ %v");
						spd_sinks.push_back(console_sink);

						std::cout << "Logger adding console sink with multi-threaded mode" << std::endl;
					}
				}
				else if (sink == "File") {
					if (threadCount == 1 && (sinkThreadingType == "st" || sinkThreadingType == "ST")) {
						auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_st>(dir + "/" + file, 0, 0);
						file_sink->set_pattern(pattern);
						spd_sinks.push_back(file_sink);

						std::cout << "Logger adding file sink with single-threaded mode" << std::endl;
					}
					else {
						auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(dir + "/" + file, 0, 0);
						file_sink->set_pattern(pattern);
						spd_sinks.push_back(file_sink);

						std::cout << "Logger adding file sink with multi-threaded mode" << std::endl;
					}
				}
			}

			// Create the async logger with the specified queue size and thread count
			spdlog::init_thread_pool(queueSize, threadCount);
			m_logger_sp = std::make_shared<spdlog::async_logger>("async_logger", spd_sinks.begin(), spd_sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
		}

		spdlog::set_default_logger(m_logger_sp);
		spdlog::flush_every(std::chrono::seconds(2));

#ifndef NDEBUG
		m_logger_sp->set_level(spdlog::level::debug);
#else
		m_logger_sp->set_level(spdlog::level::info);
#endif
	}

	std::shared_ptr<spdlog::logger> logger() { return m_logger_sp; }

	void shutdown()
	{
		if (m_logger_sp)
		{
			m_logger_sp->flush();
			spdlog::shutdown();
		}
	}

private:
	std::shared_ptr<spdlog::logger> m_logger_sp;
};

} }
