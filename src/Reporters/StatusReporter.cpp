//============================================================================
// Name        	: StatusReporter.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 23, 2023 3:04:50 PM
//============================================================================

#include <Reporters/StatusReporter.hpp>
#include <iostream>

using namespace KTN::REPORTER;

StatusReporter::StatusReporter() :
    _RPT_ACTIVE(false)
{
	Settings settings("Settings.txt");
	start(settings.getString("ArgoServer"), settings.getInteger("ArgoPushPort"));
}


StatusReporter::~StatusReporter()
{
    Stop();
}

void StatusReporter::LogMe(std::string szMsg, LogLevel::Enum level)
{
	string me = "RPT_STAT";
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void StatusReporter::start(std::string ip, int port)
{
	_endpoint = "tcp://" + ip + ":" + std::to_string(port);

    if (!_RPT_ACTIVE) {
        _RPT_ACTIVE = true;
        _thread = std::thread(&StatusReporter::reporting_thread, this);
    }
}


void StatusReporter::Stop()
{
    if (_RPT_ACTIVE) {
        _RPT_ACTIVE = false;
        _thread.join();
    }
}


void StatusReporter::Report(SessionStatus& data)
{
    _qStatus.push(data);
}


//Replaces zmqcpp with standard zmq calls.
void StatusReporter::reporting_thread() {
    int us_interval = 5 * 1000 * 1000;  // Interval in microseconds

    std::unordered_map<std::pair<std::string, int>, SessionStatus, hash_pair> _status;

    // Initialize ZeroMQ context and socket
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PUSH);

    // Log and open connection
    std::ostringstream zzz;
    zzz << "Opening connection to " << _endpoint << "...";
    LogMe(zzz.str());

    int rc = zmq_connect(socket, _endpoint.c_str());
    if (rc != 0) {
        LogMe("Failed to connect ZMQ socket", LogLevel::ERROR);
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return;
    }

    // Main loop for reporting thread
    while (_RPT_ACTIVE) {
        // Process queued status updates
        if (!_qStatus.empty()) {
            while (!_qStatus.empty() > 0) {
                SessionStatus gs;
                if (_qStatus.try_pop(gs)) {
                    _status[std::make_pair(gs.exch, gs.mktsegid)] = gs;
                }
        	}
		}

        // Skip sending if there's no status data
        if (_status.empty()) {
            usleep(us_interval);
            continue;
        }

        // Send status updates for heartbeat monitoring
        for (const auto& s : _status) {
            std::string szJson = SessionStatusFormatter::Get(s.second);
            int sent = zmq_send(socket, szJson.c_str(), szJson.size(), 0);

            if (sent == -1) {
                LogMe("Failed to send status message", LogLevel::ERROR);
            }
            usleep(100);  // Short delay between messages
        }

        usleep(us_interval);  // Interval between heartbeat transmissions
    }

    LogMe("KILLING SESS REPORTING THREAD!", LogLevel::ERROR);

    // Clean up ZeroMQ socket and context
    zmq_close(socket);
    zmq_ctx_destroy(context);
}


