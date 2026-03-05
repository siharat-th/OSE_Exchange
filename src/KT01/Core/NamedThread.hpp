//============================================================================
// Name        	: ThreadWithAffinity.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 24, 2023 11:23:04 AM
//============================================================================

#ifndef NAMEDTHREAD_HPP
#define NAMEDTHREAD_HPP

#include <pthread.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <functional>
#include <KT01/Core/Macro.hpp>
using namespace std;

/**
 * @class NamedThread
 * @brief Represents a named thread with affinity to a specific core.
 */
class NamedThread {
public:
    using ThreadFunction = std::function<void()>;
	/**
  * @brief Default constructor.
  */
	NamedThread();

    /**
  * @brief Constructs a NamedThread object with the given thread function.
  * @param function The function to be executed by the thread.
  */
    NamedThread(const ThreadFunction& function);

	/**
  * @brief Destructor.
  */
    virtual ~NamedThread();

    /**
     * @brief Creates and runs the named thread on the specified core.
     * @param name The name of the thread.
     * @param core The core to which the thread should be affinitized.
     */
    void CreateAndRun(string name, int core);

    /**
     * @brief Joins the named thread.
     */
    void Join();

private:
    /**
     * @brief Entry point for the thread.
     * @param pValue The value passed to the thread.
     * @return The thread return value.
     */
    static void *EntryPoint(void *pValue);

    pthread_t m_tid; /**< The thread ID. */
    int m_core; /**< The core to which the thread is affinitized. */
    string m_name; /**< The name of the thread. */
    ThreadFunction m_function; /**< The function to be executed by the thread. */

    /**
     * @brief The main execution logic of the thread.
     * @note Subclasses must implement this method.
     */
    /*virtual void Run() = 0;*/
    virtual void Run() {}
};

#endif // NAMEDTHREAD_HPP
