/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "radio/coap/coap_packet_static.h"
#include "radio/coap/coap_service_static.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

#define NUM_MEASUREMENTS 100

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> coap_service_t;
	typedef coap_service_t::ReceivedMessage received_message_t;
	typedef coap_service_t::coap_packet_t coap_packet_t;

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef Os::Clock::time_t time_t;

	void output_results()
	{
		debug_->debug("Results: \n");
		int i;
		for( i = 4; i < measurement_counter_; i+= 5 )
		{
			debug_->debug("%i, %i, %i, %i, %i \n",
					measurements_[i - 4], measurements_[i - 3],
					measurements_[i - 2], measurements_[i - 1],
					measurements_[i] );
		}
		// case 0 means it's already covered by the above.
		// Since order doesn't matter we don't care that the last four are in
		// reverse order.
		switch( measurement_counter_ % 5 )
		{
		case 4:
			debug_->debug("%i", measurements_[i] );
		case 3:
			debug_->debug("%i", measurements_[i - 1] );
		case 2:
			debug_->debug("%i", measurements_[i - 2] );
		case 1:
			debug_->debug("%i", measurements_[i - 3] );
		}
	}

	void tick()
	{
		tick_ = clock_->time();
	}

	void tock()
	{
		time_t now = clock_->time();
		uint16_t duration = ( ( clock_->seconds( now ) - clock_->seconds( tick_ ) ) * 1000 )
				+ ( clock_->milliseconds( now ) - clock_->milliseconds( tick_ ) );
		measurements_[measurement_counter_] = duration;
		++measurement_counter_;
		if( measurement_counter_ >= NUM_MEASUREMENTS )
		{
			output_results();
		}
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet( value );
		cservice_.init( *radio_, *timer_, *debug_, *rand_ );
		cservice_.enable_radio();
		measurement_counter_ = 0;
		//

	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* )
	{
		// TODO
		timer_->set_timer<ExampleApplication,
				&ExampleApplication::request_timed_out>( 20000, this, (void*) measurement_counter_ );
	}

	void broadcast_loop( void* counter )
	{
		if( measurement_counter_ == (uint32_t) counter )
		{
			measurements_[measurement_counter_] = -1;
			++measurement_counter_;
			if( measurement_counter_ >= NUM_MEASUREMENTS )
			{
				output_results();
			}
			else
			{
				broadcast_loop(0);
			}
		}
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( received_message_t & message )
	{
		tock();
		if( measurement_counter_ < NUM_MEASUREMENTS )
			broadcast_loop(0);
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	coap_service_t cservice_;
	time_t tick_;
	uint16_t measurement_counter_;
	int32_t measurements_[NUM_MEASUREMENTS];

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
