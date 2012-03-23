/*
 * timer_stub.h
 *
 *  Created on: Mar 21, 2012
 *      Author: wiselib
 */

#ifndef TIMER_STUB_H_
#define TIMER_STUB_H_

#include <list>
#include "util/delegates/delegate.hpp"

namespace wiselib
{
class DummyTimerModel
{
public:
	typedef delegate1<void, void*> timer_delegate_t;
	typedef uint32_t millis_t;

	class TimerEvent
	{
	public:
		TimerEvent()
		{
			time_ = 0;
			callback_ = timer_delegate_t();
			userdata_ = NULL;
		}

		TimerEvent( millis_t millis, timer_delegate_t callback, void *userdata )
		{
			time_ = millis;
			callback = callback;
			userdata_ = userdata;
		}

		~TimerEvent()
		{

		}

		millis_t time_;
		timer_delegate_t callback_;
		void* userdata_;
	};

	DummyTimerModel():
		scheduled_events_()
	{

	}

	template<typename T, void (T::*TMethod)(void*)>
	int set_timer( millis_t millis, T *obj_pnt, void *userdata )
	{
		timer_delegate_t del = timer_delegate_t::from_method<T, TMethod>(obj_pnt);
		TimerEvent tevent( millis, del, userdata );
		scheduled_events_.push_back( tevent );
	}

	size_t scheduledEvents()
	{
		return scheduled_events_.size();
	}

	TimerEvent lastEvent()
	{
		return scheduled_events_.back();
	}

	list<TimerEvent> scheduled_events_;

};
}

#endif /* TIMER_STUB_H_ */
