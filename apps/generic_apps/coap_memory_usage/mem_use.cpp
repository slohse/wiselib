/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet_static.h"
#include "radio/coap/coap_service_static.h"


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:
	typedef Os::Radio::block_data_t block_data_t;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString> coap_packet_t;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 0> coap_packet_t_0;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 1> coap_packet_t_1;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 2> coap_packet_t_2;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 3> coap_packet_t_3;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 4> coap_packet_t_4;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 5> coap_packet_t_5;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 6> coap_packet_t_6;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 7> coap_packet_t_7;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 8> coap_packet_t_8;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 10> coap_packet_t_10;
	typedef wiselib::CoapPacketStatic<Os, Os::Radio, wiselib::StaticString, 100> coap_packet_t_100;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t > coap_service_t;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_0 > coap_service_t_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_1 > coap_service_t_1;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_2 > coap_service_t_2;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_3 > coap_service_t_3;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_4 > coap_service_t_4;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_5 > coap_service_t_5;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_6 > coap_service_t_6;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_7 > coap_service_t_7;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_10 > coap_service_t_10;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t_100 > coap_service_t_100;

	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 1, 0, 0 > coap_service_t_1_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 2, 0, 0 > coap_service_t_2_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 3, 0, 0 > coap_service_t_3_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 4, 0, 0 > coap_service_t_4_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 10, 0, 0 > coap_service_t_10_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 1, 0 > coap_service_t_0_1_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 2, 0 > coap_service_t_0_2_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 3, 0 > coap_service_t_0_3_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 4, 0 > coap_service_t_0_4_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 10, 0 > coap_service_t_0_10_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 0, 1 > coap_service_t_0_0_1;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 0, 2 > coap_service_t_0_0_2;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 0, 10 > coap_service_t_0_0_10;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 10, 10, 1 > coap_service_t_10_10_1;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 10, 10, 2 > coap_service_t_10_10_2;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 10, 10, 10 > coap_service_t_10_10_10;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 0, 0, 0 > coap_service_t_0_0_0;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 1, 1, 1 > coap_service_t_1_1_1;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 2, 2, 2 > coap_service_t_2_2_2;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 3, 3, 3 > coap_service_t_3_3_3;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 4, 4, 4 > coap_service_t_4_4_4;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 5, 5, 5 > coap_service_t_5_5_5;

	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 3, 3, 1 > coap_service_t_3_3_1;
	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString, false, false, coap_packet_t, 3, 3, 0 > coap_service_t_3_3_0;



	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );

		debug_->debug( "Example Application booting!\n" );

		debug_->debug( "sizeof coap_packet_t: %i", sizeof(coap_packet_t) );
		debug_->debug( "sizeof coap_packet_t_0: %i", sizeof(coap_packet_t_0) );
		debug_->debug( "sizeof coap_packet_t_1: %i", sizeof(coap_packet_t_1) );
		debug_->debug( "sizeof coap_packet_t_2: %i", sizeof(coap_packet_t_2) );
		debug_->debug( "sizeof coap_packet_t_3: %i", sizeof(coap_packet_t_3) );
		debug_->debug( "sizeof coap_packet_t_4: %i", sizeof(coap_packet_t_4) );
		debug_->debug( "sizeof coap_packet_t_5: %i", sizeof(coap_packet_t_5) );
		debug_->debug( "sizeof coap_packet_t_6: %i", sizeof(coap_packet_t_6) );
		debug_->debug( "sizeof coap_packet_t_7: %i", sizeof(coap_packet_t_7) );
		debug_->debug( "sizeof coap_packet_t_8: %i", sizeof(coap_packet_t_8) );
		debug_->debug( "sizeof coap_packet_t_10: %i", sizeof(coap_packet_t_10) );
		debug_->debug( "sizeof coap_packet_t_100: %i", sizeof(coap_packet_t_100) );


/*		debug_->debug( "sizeof ReceivedMessage: %i", sizeof(coap_service_t::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 0: %i", sizeof(coap_service_t_0::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 1: %i", sizeof(coap_service_t_1::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 2: %i", sizeof(coap_service_t_2::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 3: %i", sizeof(coap_service_t_3::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 4: %i", sizeof(coap_service_t_4::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 5: %i", sizeof(coap_service_t_5::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 6: %i", sizeof(coap_service_t_6::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 7: %i", sizeof(coap_service_t_7::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 10: %i", sizeof(coap_service_t_10::ReceivedMessage) );
		debug_->debug( "sizeof ReceivedMessage 100: %i", sizeof(coap_service_t_100::ReceivedMessage) );

		debug_->debug( "sizeof SentMessage: %i", sizeof(coap_service_t::SentMessage) );
		debug_->debug( "sizeof SentMessage 0: %i", sizeof(coap_service_t_0::SentMessage) );
		debug_->debug( "sizeof SentMessage 1: %i", sizeof(coap_service_t_1::SentMessage) );
		debug_->debug( "sizeof SentMessage 2: %i", sizeof(coap_service_t_2::SentMessage) );
		debug_->debug( "sizeof SentMessage 3: %i", sizeof(coap_service_t_3::SentMessage) );
		debug_->debug( "sizeof SentMessage 4: %i", sizeof(coap_service_t_4::SentMessage) );
		debug_->debug( "sizeof SentMessage 5: %i", sizeof(coap_service_t_5::SentMessage) );
		debug_->debug( "sizeof SentMessage 6: %i", sizeof(coap_service_t_6::SentMessage) );
		debug_->debug( "sizeof SentMessage 7: %i", sizeof(coap_service_t_7::SentMessage) );
		debug_->debug( "sizeof SentMessage 10: %i", sizeof(coap_service_t_10::SentMessage) );
		debug_->debug( "sizeof SentMessage 100: %i", sizeof(coap_service_t_100::SentMessage) );

		debug_->debug( "sizeof CoapResource: %i", sizeof(coap_service_t::CoapResource) );
*/
/*		debug_->debug( "sizeof coap_service_t: %i", sizeof(coap_service_t) );
		debug_->debug( "sizeof coap_service_t_0: %i", sizeof(coap_service_t_0) );
		debug_->debug( "sizeof coap_service_t_10: %i", sizeof(coap_service_t_10) );
		debug_->debug( "sizeof coap_service_t_100: %i", sizeof(coap_service_t_100) );

		debug_->debug( "sizeof coap_service_t_1_0_0: %i", sizeof(coap_service_t_1_0_0) );
		debug_->debug( "sizeof coap_service_t_2_0_0: %i", sizeof(coap_service_t_2_0_0) );
		debug_->debug( "sizeof coap_service_t_3_0_0: %i", sizeof(coap_service_t_3_0_0) );
		debug_->debug( "sizeof coap_service_t_4_0_0: %i", sizeof(coap_service_t_4_0_0) );
		debug_->debug( "sizeof coap_service_t_10_0_0: %i", sizeof(coap_service_t_10_0_0) );
		debug_->debug( "sizeof coap_service_t_0_1_0: %i", sizeof(coap_service_t_0_1_0) );
		debug_->debug( "sizeof coap_service_t_0_2_0: %i", sizeof(coap_service_t_0_2_0) );
		debug_->debug( "sizeof coap_service_t_0_3_0: %i", sizeof(coap_service_t_0_3_0) );
		debug_->debug( "sizeof coap_service_t_0_4_0: %i", sizeof(coap_service_t_0_4_0) );
*/		debug_->debug( "sizeof coap_service_t_0_10_0: %i", sizeof(coap_service_t_0_10_0) );
/*		debug_->debug( "sizeof coap_service_t_0_0_1: %i", sizeof(coap_service_t_0_0_1) );
		debug_->debug( "sizeof coap_service_t_0_0_2: %i", sizeof(coap_service_t_0_0_2) );
		debug_->debug( "sizeof coap_service_t_0_0_10: %i", sizeof(coap_service_t_0_0_10) );
		debug_->debug( "sizeof coap_service_t_10_10_1: %i", sizeof(coap_service_t_10_10_1) );
		debug_->debug( "sizeof coap_service_t_10_10_2: %i", sizeof(coap_service_t_10_10_2) );
*/		debug_->debug( "sizeof coap_service_t_10_10_10: %i", sizeof(coap_service_t_10_10_10) );
/*		debug_->debug( "sizeof coap_service_t_0_0_0: %i", sizeof(coap_service_t_0_0_0) );
		debug_->debug( "sizeof coap_service_t_1_1_1: %i", sizeof(coap_service_t_1_1_1) );
		debug_->debug( "sizeof coap_service_t_2_2_2: %i", sizeof(coap_service_t_2_2_2) );
		debug_->debug( "sizeof coap_service_t_3_3_3: %i", sizeof(coap_service_t_3_3_3) );
		debug_->debug( "sizeof coap_service_t_4_4_4: %i", sizeof(coap_service_t_4_4_4) );
		debug_->debug( "sizeof coap_service_t_5_5_5: %i", sizeof(coap_service_t_5_5_5) );
*/
		debug_->debug( "sizeof coap_service_t_3_3_1: %i", sizeof(coap_service_t_3_3_1) );
		debug_->debug( "sizeof coap_service_t_3_3_0: %i", sizeof(coap_service_t_3_3_0) );

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
