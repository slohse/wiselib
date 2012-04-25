/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "radio/coap/coap_packet_static.h"
#include "radio/coap/coap_service_static.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>::self_t coapradio_t;

	void read_temperature()
	{
		temperature_ = 23;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		cradio_.init( *radio_, *timer_, *debug_, *rand_ );
		cradio_.enable_radio();

		temperature_ = -5;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );

		debug_->debug( "node %x > Temperature CoAP Service booting\n", radio_->id() );

		temp_uri_path_ = wiselib::StaticString("temperature");

		cradio_.reg_resource_callback< ExampleApplication,
				&ExampleApplication::receive_coap>( temp_uri_path_, this );

//		recv_callback_id_ = radio_->reg_recv_callback<ExampleApplication,
//					&ExampleApplication::receive_radio_message > ( this );

	}
	// --------------------------------------------------------------------
	void temperature_loop( void* )
	{
		read_temperature();
		timer_->set_timer<ExampleApplication,
							&ExampleApplication::temperature_loop>( 10000, this, 0 );
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		debug_->debug( "node %x > received raw message from %x, len %i, pointer %x \n",
				radio_->id(), from, len, (uint32_t) buf );
//		wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t packet;
//		packet.parse_message(buf, len);
//		debug_->debug( "Node %x -- CoapRadio::receive> packet: version %x type %x code %x msg_id %x\n",
//				radio_->id(), packet.version(), packet.type(), packet.code() , packet.msg_id() );
/*		char strbuf[200];
		size_t strlen = 0;
		for(size_t i = 0; i < len / 4; ++i)
		{
			strlen += sprintf( strbuf + strlen - 1, "%x, %x, %x, %x, ",
					(int) *(buf + i*4),
					(int) *(buf + i*4 + 1),
					(int) *(buf + i*4 + 2),
					(int) *(buf + i*4 + 3) );
		}
		debug_->debug( "node %x > content: %s\n", radio_->id(), strbuf );
		*/
	}

	void receive_coap( coapradio_t::ReceivedMessage & message )
	{
		coapradio_t::coap_packet_t & packet = message.message();

		if( packet.is_request() && packet.uri_path() == temp_uri_path_ )
		{
			debug_->debug( "node %x > received request for temperature\n", radio_->id() );
			cradio_.reply( message, (uint8_t*) temperature_str_, temperature_str_len_ );
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;

	int recv_callback_id_;

	coapradio_t cradio_;

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
