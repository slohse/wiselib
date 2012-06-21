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

//	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, true, false, wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString>, 5, 5, 0 > coap_service_t;
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
		cservice_.init( *radio_, *timer_, *rand_, *debug_);
		cservice_.enable_radio();
		//

		server_id_ = 0x2014;
		temp_uri_path_ = wiselib::StaticString("temperature");

		debug_->debug( "node %x > Starting 'temperature' client\n", radio_->id() );

		timer_->set_timer<ExampleApplication,
				&ExampleApplication::broadcast_loop>( 1000, this, 0 );
	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* )
	{
		debug_->debug( "node %x > sending GET to %i\n", radio_->id(), server_id_ );
		cservice_.get< ExampleApplication, &ExampleApplication::receive_coap>( server_id_, temp_uri_path_, wiselib::StaticString(), this );
		timer_->set_timer<ExampleApplication,
				&ExampleApplication::broadcast_loop>( 3000, this, NULL );
	}
	// --------------------------------------------------------------------
	void receive_coap( received_message_t & message )
	{
		debug_->debug("node %x -- receive_coap> Temperature at Server %x is %s \n",
				radio_->id(), message.correspondent(), (char*) message.message().data() );
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	coap_service_t cservice_;
	time_t tick_;

	wiselib::StaticString temp_uri_path_;
	node_id_t server_id_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
