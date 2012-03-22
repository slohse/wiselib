/*
 * timer_stub.h
 *
 *  Created on: Mar 21, 2012
 *      Author: wiselib
 */

#ifndef TIMER_STUB_H_
#define TIMER_STUB_H_

#include <list>

namespace wiselib
{
class DummyTimerModel
{
public:
	typedef uint32_t millis_t;

	class TimerEvent
	{
	public:
		TimerEvent()
		{
			time_ = 0;
			obj_ptr_ = NULL;
			userdata_ = NULL;
		}

		TimerEvent( millis_t millis, void *obj_pnt, void *userdata )
		{
			time_ = millis;
			obj_ptr_ = obj_pnt;
			userdata_ = userdata;
		}

		~TimerEvent()
		{

		}

		millis_t time_;
		void* obj_ptr_;
		void* userdata_;
	};

	DummyTimerModel():
		scheduled_events_()
	{

	}

	template<typename T, void (T::*TMethod)(void*)>
	int set_timer( millis_t millis, T *obj_pnt, void *userdata )
	{
		TimerEvent tevent( millis, obj_pnt, userdata );
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

private:
	list<TimerEvent> scheduled_events_;

};
}

#endif /* TIMER_STUB_H_ */
