/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

#define COAP_APP_DEBUG


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		//
		debug_->debug( "node %x > Temperature CoAP Client booting\n", radio_->id() );

		server_id_ = 0x2015;
		temp_uri_path_ = wiselib::StaticString("temperature");

		// request temperature every second
		// (yes, even though the server only measures every 10 seconds - we want
		// to evaluate the protocol, not the temperature sensor...)
		timer_->set_timer<ExampleApplication,
							&ExampleApplication::get_temperature_loop>( 1000, this, 0 );

	}
	// --------------------------------------------------------------------
	void get_temperature_loop( void* )
	{
		cradio_.get< ExampleApplication, &ExampleApplication::receive_coap>( server_id_, temp_uri_path_, wiselib::StaticString(), this );
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>::ReceivedMessage & message )
	{
		wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t & packet = message.message();
		if( packet.is_response() )
		{
			if( packet.data_length() > 0 && packet.data_length() < 5)
			{
				memcpy(temperature_str_, packet.data(), packet.data_length() );
				temperature_str_[ packet.data_length() ] = '\0';
				debug_->debug( "node %x > Temperature: %s °C\n", radio_->id(), temperature_str_ );
			}
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;

	wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> cradio_;

	wiselib::StaticString temp_uri_path_;
	node_id_t server_id_;

	char temperature_str_[5];

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
