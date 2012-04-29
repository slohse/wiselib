/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#define COAP_PREFACE_MSG_ID 1

#include "radio/coap/coap_packet_static.h"
#include "radio/coap/coap_service_static.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

//	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, true, false, wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString>::coap_packet_t, COAPRADIO_SENT_LIST_SIZE, COAPRADIO_RECEIVED_LIST_SIZE, COAPRADIO_RESOURCES_SIZE> coap_service_t;
	typedef wiselib::CoapServiceStatic<Os> coap_service_t;
	typedef coap_service_t::ReceivedMessage received_message_t;
	typedef coap_service_t::coap_packet_t coap_packet_t;

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef Os::Clock::time_t time_t;

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		value.radio().hardware_radio().set_channel(11);
		cservice_.init( *radio_, *timer_, *rand_ );
		cservice_.enable_radio();

		temperature_ = 23;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );

		debug_->debug( "node %x > Temperature CoAP Service booting\n", radio_->id() );

		temp_uri_path_ = wiselib::StaticString("temperature");

		cservice_.reg_resource_callback< ExampleApplication,
			&ExampleApplication::receive_coap>( temp_uri_path_, this );

	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( received_message_t & message )
	{
//		debug_->debug( "node %x > received coap", radio_->id() );
		coap_packet_t & packet = message.message();

		if( packet.is_request() && packet.uri_path() == temp_uri_path_ )
		{
//			debug_->debug( "node %x > received request for temperature\n", radio_->id() );
			cservice_.reply( message, (uint8_t*) temperature_str_, temperature_str_len_ );
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;

	coap_service_t cservice_;

	wiselib::StaticString temp_uri_path_;
	int8_t temperature_;
	char temperature_str_[5];
	size_t temperature_str_len_;


};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
