//============================================================================
// Name        	: TimerCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 8, 2023 4:54:12 PM
//============================================================================

#ifndef SRC_ALGOCALCULATORS_TIMERCALLBACK_HPP_
#define SRC_ALGOCALCULATORS_TIMERCALLBACK_HPP_

#include <stdint.h>

struct TimerInterval{
	enum Enum : uint8_t
	{
		CALCULATING,TRIGGERED, MINSECS, MAXSECS
	};
};

struct TimerState{
	enum Enum : uint8_t
	{
		ERROR,WAIT,START,END
	};
};

/**
 * @brief The TimerCallback class is an abstract base class for timer callback functionality.
 */
class TimerCallback {
public:
	TimerCallback(){};
	virtual ~TimerCallback(){};

	/**
  * @brief This method is called when the timer state changes.
  * @param state The new state of the timer.
  */
	virtual void onTimerStateChange(TimerState::Enum state) = 0;

	/**
  * @brief This method is called when the timer interval changes.
  * @param interval The new interval of the timer.
  */
	virtual void onTimerInterval(TimerInterval::Enum interval) = 0;

};

#endif /* SRC_ALGOCALCULATORS_TIMERCALLBACK_HPP_ */
