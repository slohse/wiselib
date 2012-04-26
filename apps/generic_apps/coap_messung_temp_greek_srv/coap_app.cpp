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

	void tick()
	{
		tick_ = clock_->time();
		++measurement_counter_;
	}

	void tock()
	{
		time_t now = clock_->time();
		int duration = ( ( clock_->seconds( now ) - clock_->seconds( tick_ ) ) * 1000 )
				+ ( clock_->milliseconds( now ) - clock_->milliseconds( tick_ ) );
		debug_->debug("duration %i ms", duration);
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet( value );
		cservice_.init( *radio_, *timer_, *debug_, *rand_ );
		cservice_.enable_radio();
		measurement_counter_ = 0;
		//

		server_id_ = 0x2015;

	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* counter )
	{
		if( measurement_counter_ < NUM_MEASUREMENTS)
		{
			debug_->debug("Tick");
			tick();
			cservice_.get< ExampleApplication, &ExampleApplication::receive_coap>( server_id_, temp_uri_path_, wiselib::StaticString(), this );
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
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( received_message_t & message )
	{
		tock();
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	node_id_t server_id_;
	coap_service_t cservice_;
	time_t tick_;
	uint16_t measurement_counter_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
