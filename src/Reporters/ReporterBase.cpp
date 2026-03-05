//============================================================================
// Name        	: ReporterBase.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 23, 2023 3:04:50 PM
//============================================================================

#include <Reporters/ReporterBase.hpp>
#include <iostream>

using namespace KTN::REPORTER;

ReporterBase::ReporterBase(string name, int sleepus) :
    _name(name), _sleepus(sleepus)
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}


ReporterBase::~ReporterBase()
{
    Stop();
}

void ReporterBase::Start()
{
	Settings settings("Settings.txt");
	start(settings.getString("ArgoServer"), settings.getInteger("ArgoPushPort"));
}

void ReporterBase::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void ReporterBase::start(std::string ip, int port)
{
	_endpoint = "tcp://" + ip + ":" + std::to_string(port);

    if (_ACTIVE.load(std::memory_order_relaxed) == false) {
        _rptthread = std::thread(&ReporterBase::reporting_thread, this);
        _msgthread = std::thread(&ReporterBase::messaging_thread, this);
    }
}

void ReporterBase::Stop()
{
	 if (_ACTIVE.load(std::memory_order_relaxed) == true) {
		 _ACTIVE.store(false, std::memory_order_relaxed);
        _rptthread.join();
        _msgthread.join();
    }
}

void ReporterBase::DoWork()
{
	LogMe("ERROR: DoWork() NOT IMPLEMENTED. ASS.", LogLevel::ERROR);
}

void ReporterBase::reporting_thread()
{
	_ACTIVE.store(true, std::memory_order_relaxed);
	while(_ACTIVE.load(std::memory_order_relaxed) == true)
    {
        DoWork();
        usleep(_sleepus);
    }

   LogMe("KILLING SESS REPORTING THREAD!", LogLevel::ERROR);

}

void ReporterBase::messaging_thread() {
    // Initialize the ZeroMQ context and socket
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PUSH);

    // Log connection attempt
    std::ostringstream zzz;
    zzz << "ReporterBase Opening connection to " << _endpoint << "...";
    LogMe(zzz.str());

    // Connect to the endpoint
    int rc = zmq_connect(socket, _endpoint.c_str());
    if (rc != 0) {
        LogMe("Failed to connect ZMQ socket", LogLevel::ERROR);
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return;
    }

    // Mark the thread as active
    _ACTIVE.store(true, std::memory_order_relaxed);

    // Main message sending loop
    while (_ACTIVE.load(std::memory_order_relaxed)) {
        // Process JSON messages from the queue
        while (!_qJson.empty()) {
            std::string szJson;
            if (_qJson.try_pop(szJson)) {
                // Send JSON message using ZeroMQ
                LOGINFO("[ReporterBase] SENDING JSON: {}", szJson);
                int sent = zmq_send(socket, szJson.c_str(), szJson.size(), 0);
                if (sent == -1) {
                    LogMe("ZMQ REPORTER FAILED TO SEND: " + _name, LogLevel::ERROR);
                }
                usleep(10000);  // Sleep for 10 ms after sending
            }
        }

        usleep(_sleepus);  // Sleep for a specified time before next iteration
    }

    LogMe("KILLING REPORTER BASE THREAD!", LogLevel::ERROR);

    // Clean up the ZeroMQ socket and context
    zmq_close(socket);
    zmq_ctx_destroy(context);
}

void KTN::REPORTER::ReporterBase::SendJson(string json) {
	_qJson.push(json);
}
