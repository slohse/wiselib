/*
 * Simple Wiselib Example
 */
#define DEBUG_COAPRADIO_TEST_XX
#include "external_interface/external_interface.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"
#include "string.h"
#include <isense/modules/environment_module/environment_module.h>
#include <isense/modules/environment_module/temp_sensor.h>

#define COAP_APP_DEBUG


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;

	void read_temperature()
	{
		temperature_ = em_->temp_sensor()->temperature();
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

		temperature_ = -1;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );
		//
		debug_->debug( "node %x > Temperature CoAP Service booting\n", radio_->id() );
		debug_->debug( "node %x > sizeof(coap_packet_t) = %i, sizeof(coapradio_t) = %i",
				radio_->id(),
				sizeof(wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>),
				sizeof(wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t));

		CoapCode testcode = COAP_CODE_EMPTY;
		debug_->debug( "Code Empty (0) = %i, bzw %x\n", testcode, testcode);
		testcode = COAP_CODE_GET;
		debug_->debug( "Code GET (1) = %i, bzw %x\n", testcode, testcode);
		testcode = COAP_CODE_CONTENT;
		debug_->debug( "Code Content (69) = %i, bzw %x\n", testcode, testcode);
		testcode = COAP_CODE_NOT_FOUND;
		debug_->debug( "Code Not Found (132) = %i, bzw %x\n", testcode, testcode);
		testcode = (CoapCode) -1;
		debug_->debug( "Code (-1) = %i, bzw %x\n", testcode, testcode);

		temp_uri_path_ = wiselib::StaticString("temperature");
		em_ = new isense::EnvironmentModule(value);

		if (em_->temp_sensor() != NULL)
		{
			em_->temp_sensor()->enable();
			read_temperature();
			debug_->debug( "node %x > Temperature Sensor enabled, current Temperature: %i °C\n", radio_->id(), temperature_ );
			// measure temperature every 10 seconds
			timer_->set_timer<ExampleApplication,
					&ExampleApplication::temperature_loop>( 10000, this, 0 );
			cradio_.reg_resource_callback< ExampleApplication, &ExampleApplication::receive_coap>( temp_uri_path_, this );
		}

		recv_callback_id_ = radio_->reg_recv_callback<ExampleApplication,
					&ExampleApplication::receive_radio_message > ( this );

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

	void receive_coap( wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>::ReceivedMessage & message )
	{
		debug_->debug( "node %x > received message\n", radio_->id() );
		wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t & packet = message.message();

		if( packet.is_request() && packet.uri_path() == temp_uri_path_ )
		{
			debug_->debug( "node %x > is request for temperature\n", radio_->id() );
			cradio_.reply( message, (uint8_t*) temperature_str_, temperature_str_len_ );
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;

	int recv_callback_id_;
	isense::EnvironmentModule* em_;

	wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> cradio_;

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
