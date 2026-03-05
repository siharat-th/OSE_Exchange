//============================================================================
// Name        	: NamedThread.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 24, 2023 11:23:04 AM
//============================================================================

#include <KT01/Core/NamedThread.hpp>
#include <iostream>

NamedThread::NamedThread() : m_tid(0) {}

NamedThread::NamedThread(const ThreadFunction& function)
    : m_tid(0), m_core(0), m_function(function) {
}

NamedThread::~NamedThread() {
    Join();
}

void NamedThread::CreateAndRun(string name, int core) {
    m_core = core;
    m_name = name;
    if (m_name.length() > 15)
    {
    	m_name = m_name.substr(0, 14);
    }
    int rc = pthread_create(&m_tid, nullptr, NamedThread::EntryPoint, this);
    if (rc != 0) {
        std::cout << "ERROR CREATING THREAD RC = " << rc << std::endl;
    }
}

void NamedThread::Join() {
    if (m_tid != 0) {
        pthread_join(m_tid, nullptr);
    }
}

void *NamedThread::EntryPoint(void *pValue) {
    pthread_t current_thread = pthread_self();
    std::string thread_name = static_cast<NamedThread*>(pValue)->m_name;

    auto* namedThread = static_cast<NamedThread*>(pValue);

    int irc = pthread_setname_np(current_thread,
    		static_cast<NamedThread*>(pValue)->m_name.c_str());
	if (irc != 0) {
		cout << "ERROR NAMING THREAD: " << thread_name << endl;
		exit(1);
	}

    int core = static_cast<NamedThread*>(pValue)->m_core;
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(core, &cpus);
    irc = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);
    if (irc!=0)
    {
    	cout << "ERROR SETTING THREAD AFFINITY: " << core << endl;
    	exit(1);
    }

    if (namedThread->m_function) {
        namedThread->m_function();
    }
    else {
        static_cast<NamedThread*>(pValue)->Run();
    }

    return nullptr;
}
