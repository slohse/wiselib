/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "radio/coap/coap_packet_static.h"
#include "radio/coap/coap_service_static.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

#define NUM_MEASUREMENTS	10000
#define MAX_DURATION	25
#define DISTRIBUTION_SIZE	MAX_DURATION + 2

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString> coap_packet_t;

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
		++received_counter_;
		int duration = ( ( clock_->seconds( now ) - clock_->seconds( tick_ ) ) * 1000 )
				+ ( clock_->milliseconds( now ) - clock_->milliseconds( tick_ ) );
		if( duration > MAX_DURATION )
		{
			++duration_distribution_[ MAX_DURATION + 1 ];
			if( duration > max_ )
				max_ = duration;
		}
		else
		{
			++duration_distribution_[ duration ];
		}
	}

	void send_request()
	{
		coap_packet_t packet;
		block_data_t buf[100];
		uint8_t buf_len;

		packet.init();
		packet.set_type( COAP_MSG_TYPE_NON );
		packet.set_code( COAP_CODE_GET );
		packet.set_msg_id( mid_++ );

		packet.set_uri_path( temp_uri_path_ );

		buf_len = packet.serialize( buf );

		radio_->send( server_id_, buf_len, buf );
	}

	void print_results()
	{
		debug_->debug( "packets sent: %i. packets received: %i. Max time: %i.", measurement_counter_, received_counter_, max_ );
		debug_->debug( "0-9: %i - %i - %i - %i - %i - %i - %i - %i - %i - %i ",
				duration_distribution_[0], duration_distribution_[1], duration_distribution_[2], duration_distribution_[3], duration_distribution_[4],
				duration_distribution_[5], duration_distribution_[6], duration_distribution_[7], duration_distribution_[8], duration_distribution_[9]);
		debug_->debug( "10-19: %i - %i - %i - %i - %i - %i - %i - %i - %i - %i ",
				duration_distribution_[10], duration_distribution_[11], duration_distribution_[12], duration_distribution_[13], duration_distribution_[14],
				duration_distribution_[15], duration_distribution_[16], duration_distribution_[17], duration_distribution_[18], duration_distribution_[19]);
		debug_->debug( "20-25,longer: %i - %i - %i - %i - %i - %i - %i",
				duration_distribution_[20], duration_distribution_[21], duration_distribution_[22], duration_distribution_[23], duration_distribution_[24],
				duration_distribution_[25], duration_distribution_[26]);
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet( value );
		value.radio().hardware_radio().set_channel(11);
		measurement_counter_ = 0;
		received_counter_ = 0;
		max_ = 0;
		for( size_t i = 0; i < DISTRIBUTION_SIZE; ++i)
		{
			duration_distribution_[i] = 0;
		}
		print_results();
		//

		mid_ = (*rand_)();

		server_id_ = 0x2015;
		temp_uri_path_ = wiselib::StaticString("temperature");

		debug_->debug( "node %x > Starting measurement 'temperature'. Making %i measurements, starting in 5 seconds\n", radio_->id(), NUM_MEASUREMENTS );

		timer_->set_timer<ExampleApplication,
				&ExampleApplication::broadcast_loop>( 5000, this, 0 );

		radio_->reg_recv_callback<ExampleApplication,
				&ExampleApplication::receive_radio_message > ( this );
	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* )
	{
		if( received_counter_ < NUM_MEASUREMENTS && measurement_counter_ < ( NUM_MEASUREMENTS * 2))
		{
			if( (received_counter_ % 50) == 0)
			{
				debug_->debug( "%i messages received ", received_counter_ );
			}
//			debug_->debug("Tick");
			tick();
			send_request();
			timer_->set_timer<ExampleApplication,
					&ExampleApplication::broadcast_loop>( 100, this, NULL );
		}
		else
		{
			print_results();
			debug_->debug("Program ends");
		}
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		coap_packet_t packet;
		int status = packet.parse_message( buf, len );
//		debug_->debug("receive, status %i", status );
		if( status == 0 )
			tock();
	}

private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	time_t tick_;

	wiselib::StaticString temp_uri_path_;
	node_id_t server_id_;

	uint16_t mid_;

	size_t measurement_counter_;
	size_t received_counter_;

	uint16_t duration_distribution_[ DISTRIBUTION_SIZE ];
	uint16_t max_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}