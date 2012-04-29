/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"

#include "algorithms/coap/coap.h"
#include "algorithms/coap/packet.h"
#include "algorithms/coap/resource.h"

#include "util/pstl/static_string.h"

#include "stdlib.h"

typedef wiselib::OSMODEL Os;
typedef wiselib::Coap<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand> coap_t;

class ExampleApplication
{
public:
	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef Os::Clock::time_t time_t;

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet( value );
		clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet( value );

		value.radio().hardware_radio().set_channel(11);

		rand_->srand( radio_->id() );
		mid_ = ( uint16_t )rand_->operator()( 65536 / 2 );
		//

		temperature_ = 23;
		temperature_str_len_ = sprintf( temperature_str_, "%i", temperature_ );

		uint8_t i;
		// reg_resource(name, fast_response, observable_time - zero for non observable, expected_len, content_type )
		for( i = 0; i < CONF_MAX_RESOURCES; i++ )
		{
			resources[i].init();
		}
		i = 0;
		resources[i].set_method( 0, GET );
		resources[i].reg_callback<ExampleApplication, &ExampleApplication::receive_coap>( this, 0 );
		resources[i].reg_resource( "temperature", false, 0, 1, TEXT_PLAIN );


		coap_.init( *radio_, *timer_, *debug_, mid_, resources );
		server_id_ = 0x2015;

//		debug_->debug( "node %x > Starting 'temperature' server\n", radio_->id() );

		radio_->reg_recv_callback<ExampleApplication, &ExampleApplication::receive_radio_message>( this );

	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* counter )
	{

	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		if ( buf[0] == WISELIB_MID_COAP )
		{
			//debug_->debug( "\n" );
//			debug_->debug( "Node %x received msg from %x msg type: CoAP length: %d", radio_->id(), from, len );
//			debug_hex( buf, len );
			coap_.receiver( &len, buf, &from );
		}
	}

	char* receive_coap( uint8_t method )
	{
//		debug_->debug( "receive coap, method %i", method );
		return temperature_str_;
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
	Os::Clock::self_pointer_t clock_;

	node_id_t server_id_;

	int8_t temperature_;
	char temperature_str_[5];
	size_t temperature_str_len_;

	coap_t coap_;
	coap_packet_t packet;
	resource_t resources[CONF_MAX_RESOURCES];
	uint16_t mid_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
