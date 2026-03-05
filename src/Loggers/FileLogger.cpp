//============================================================================
// Name        	: FileLogger.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 28, 2023 11:24:16 AM
//============================================================================

#include <Loggers/FileLogger.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <sstream>
#include <iomanip>
#include <chrono>
using namespace std;

FileLogger::FileLogger(){
}

void FileLogger::log(const std::string& message) {
    logger->info(message);
}

void FileLogger::Init(const std::string& log_dir, const std::string& prefix, size_t max_size, bool rotate_daily) {

	std::string file_name = log_dir + "/" + prefix + ".log";// + "_%Y%m%d-%H%M%S";
    std::string svcname = "log-" + prefix;

    if (rotate_daily) {
        // Daily rotation at midnight
        logger = spdlog::daily_logger_mt(svcname, file_name, 0, 0);
    } else {
        // Size-based rotation
        logger = spdlog::rotating_logger_mt(svcname, file_name, max_size, 3);
    }


    logger->set_level(spdlog::level::info); // Set log level to info
    //logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] %v");
    logger->set_pattern("%v");
    logger->flush_on(spdlog::level::info);
}
