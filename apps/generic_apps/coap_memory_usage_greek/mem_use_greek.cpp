/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "algorithms/coap/coap.h"
#include "algorithms/coap/packet.h"
#include "algorithms/coap/resource.h"


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:
	typedef Os::Radio::block_data_t block_data_t;
	typedef wiselib::Coap<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand> coap_t;

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );

		debug_->debug( "Example Application booting!\n" );

		debug_->debug( "sizeof coap_packet_t: %i", sizeof(coap_packet_t) );
		debug_->debug( "sizeof resource_t: %i", sizeof(resource_t) );
		debug_->debug( "sizeof coap_t: %i", sizeof(coap_t) );

	}
	// --------------------------------------------------------------------
	void broadcast_loop( void* )
	{

	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{

	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;

	wiselib::StaticString somestring_;
	wiselib::StaticString other_string_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
