/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "util/pstl/static_string.h"
#include "util/pstl/string_dynamic.h"

#include "util/allocators/malloc_free_allocator.h"

#include "stdlib.h"

#define COAP_APP_DEBUG


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;
	typedef wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_t coap_packet_t;
	typedef wiselib::OpaqueData opaque_data_t;

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );

		coap_packet_t packet1;

		packet1.set_type( COAP_MSG_TYPE_CON );

		for(uint8_t i = 1; i < 32; ++i )
		{
			packet1.set_code( (CoapCode) i );
			if( !packet1.is_request() )
			{
				debug_->debug(" Error, Line %i, Packet should be request but isn't (Code %i).\n", __LINE__, i );
			}
			if( packet1.is_response() )
			{
				debug_->debug(" Error, Line %i, Packet is response but shouldn't be (Code %i).\n", __LINE__, i );
			}
		}

		for(uint8_t i = 32; i < 64; ++i )
		{
			packet1.set_code( (CoapCode) i );
			if( packet1.is_request() )
			{
				debug_->debug(" Error, Line %i, Packet is request but shouldn't be (Code %i).\n", __LINE__, i );
			}
			if( packet1.is_response() )
			{
				debug_->debug(" Error, Line %i, Packet is response but shouldn't be (Code %i).\n", __LINE__, i );
			}
		}

		for(uint8_t i = 64; i < 192; ++i )
		{
			packet1.set_code( (CoapCode) i );
			if( packet1.is_request() )
			{
				debug_->debug(" Error, Line %i, Packet is request but shouldn't be (Code %i).\n", __LINE__, i );
			}
			if( !packet1.is_response() )
			{
				debug_->debug(" Error, Line %i, Packet should be response but isn't (Code %i).\n", __LINE__, i );
			}
		}

		for(uint8_t i = 192; i < 255; ++i )
		{
			packet1.set_code( (CoapCode) i );
			if( packet1.is_request() )
			{
				debug_->debug(" Error, Line %i, Packet is request but shouldn't be (Code %i).\n", __LINE__, i );
			}
			if( packet1.is_response() )
			{
				debug_->debug(" Error, Line %i, Packet is response but shouldn't be (Code %i).\n", __LINE__, i );
			}
		}

		// the dreaded 4.04 ;)
		packet1.set_code( COAP_CODE_NOT_FOUND );

		packet1.set_msg_id( 0x5555 );

		if( packet1.msg_id() != 0x5555 )
		{
			debug_->debug(" Error, Line %i, msg id is wrong.\n", __LINE__ );
		}

		if( packet1.what_options_are_set() != 0 )
		{
			debug_->debug(" Error, Line %i, options are set, %x.\n", __LINE__, packet1.what_options_are_set() );
		}
		debug_->debug("setting token, sizeof uint64_t: %i\n", sizeof(uint64_t) );
		uint64_t token64b = 0xA0A0A0A0A0A0A0A0;
		debug_->debug("I didn't break the uint...\n");
		opaque_data_t opaqued_token( (uint8_t*) &token64b, 8 );
		debug_->debug("I didn't break OpaqueData\n");
		packet1.set_token(opaqued_token);

		if( packet1.what_options_are_set() != ( 1 << COAP_OPT_TOKEN ) )
		{
			debug_->debug(" Error, Line %i, options other than token are set, %x.\n", __LINE__, packet1.what_options_are_set() );
		}


	}
	// --------------------------------------------------------------------
	void broadcast_loop( void* )
	{

	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( Os::Radio::node_id_t from, const wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_r packet )
	{

	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
