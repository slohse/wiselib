/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "algorithms/coap/coap.h"
#include "algorithms/coap/packet.h"
#include "algorithms/coap/resource.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

#define NUM_MEASUREMENTS 100

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
		time_t now = clock_->time();
		int duration = ( ( clock_->seconds( now ) - clock_->seconds( tick_ ) ) * 1000 )
				+ ( clock_->milliseconds( now ) - clock_->milliseconds( tick_ ) );
		debug_->debug("duration %i ms", duration);
	}

	void send_request()
	{
		coap_packet_t packet;
		char path[] = "temperature";
		block_data_t buf[100];
		uint8_t buf_len;

		packet.init();
		packet.set_type( CON );
		packet.set_code( PUT );
		packet.set_mid( mid_++ );

		packet.set_uri_path_len( sizeof( path ) - 1 );
		packet.set_uri_path( path );

		packet.set_option( URI_PATH );
		buf_len = packet.packet_to_buffer( buf );

		radio_->send( Os::Radio::BROADCAST_ADDRESS, buf_len, buf );
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

		mid_ = (*rand_)();

		radio_->reg_recv_callback<ExampleApplication, &ExampleApplication::receive_radio_message>( this );

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
			send_request();
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
		if( from == server_id_ )
			tock();
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

	uint16_t mid_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
