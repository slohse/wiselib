/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "stdlib.h"

#define NUM_MEASUREMENTS 50

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef Os::Clock::time_t time_t;

	void tick()
	{
		tick_ = clock_->time();
		++measurement_counter_;
	}

	void tock()
	{
		if(radio_->id() != server_id_ )
		{
			time_t now = clock_->time();
			int duration = ( ( clock_->seconds( now ) - clock_->seconds( tick_ ) ) * 1000 )
					+ ( clock_->milliseconds( now ) - clock_->milliseconds( tick_ ) );
			debug_->debug("duration %i ms", duration);
		}
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet( value );
		measurement_counter_ = 0;
		//

		server_id_ = 0x2015;
		ping_ = 23;
		pong_ = 42;

		radio_->reg_recv_callback<ExampleApplication,
				&ExampleApplication::receive_radio_message > ( this );

		if(radio_->id() != server_id_ )
		timer_->set_timer<ExampleApplication,
				&ExampleApplication::broadcast_loop>( 1000, this, NULL );
	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* counter )
	{
		if( measurement_counter_ < NUM_MEASUREMENTS)
		{
			debug_->debug("Tick");
			tick();
			radio_->send( server_id_, sizeof(ping_), (block_data_t*) &ping_ );
			timer_->set_timer<ExampleApplication,
					&ExampleApplication::broadcast_loop>( 5000, this, NULL );
		}
		else
		{
			debug_->debug("Program ends");
		}
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		if( len == sizeof(uint8_t) )
		{
			if( *buf == ping_ )
				radio_->send( from, sizeof(pong_), (block_data_t*) &pong_ );
			else if ( *buf == pong_ )
				tock();
		}

	}

private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	node_id_t server_id_;
	time_t tick_;
	uint16_t measurement_counter_;
	uint8_t ping_;
	uint8_t pong_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
