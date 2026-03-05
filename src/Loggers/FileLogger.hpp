//============================================================================
// Name        	: FIleLogger.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 28, 2023 11:24:16 AM
//============================================================================

#ifndef SRC_LOGGERS_FILELOGGER_HPP_
#define SRC_LOGGERS_FILELOGGER_HPP_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <memory>
#include <string>

/**
 * @class FileLogger
 * @brief A class for logging messages to a file.
 */
class FileLogger {
public:
    /**
     * @brief Default constructor for FileLogger.
     */
	FileLogger();

    /**
     * @brief Initializes the FileLogger with the specified parameters.
     * @param log_dir The directory where the log files will be stored.
     * @param prefix The prefix to be added to the log file names.
     * @param max_size The maximum size of each log file in bytes.
     * @param rotate_daily Flag indicating whether to rotate log files daily.
     */
    void Init(const std::string& log_dir, const std::string& prefix, size_t max_size, bool rotate_daily);

    /**
     * @brief Logs a message to the file.
     * @param message The message to be logged.
     */
    void log(const std::string& message);

private:
    std::shared_ptr<spdlog::logger> logger; /**< The logger object used for logging. */
};

#endif /* SRC_LOGGERS_FILELOGGER_HPP_ */
