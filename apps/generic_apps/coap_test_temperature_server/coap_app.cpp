/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

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

		temperature_ = -1;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );
		//
		debug_->debug( "node %x > Temperature CoAP Service booting\n", radio_->id() );

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

	}

	void receive_coap( wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>::ReceivedMessage & message )
	{
		wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t & packet = message.message();

		if( packet.is_request() && packet.uri_path() == temp_uri_path_ )
		{
			cradio_.reply( message, (uint8_t*) temperature_str_, temperature_str_len_ );
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;

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
