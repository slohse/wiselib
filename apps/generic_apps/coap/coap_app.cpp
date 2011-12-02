/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "util/pstl/static_string.h"


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
   public:
      void init( Os::AppMainParameter& value )
      {
         radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
         timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
         debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
//
         debug_->debug( "Example Application booting!\n" );
//
//         radio_->reg_recv_callback<ExampleApplication,
//                                   &ExampleApplication::receive_radio_message>( this );
#define EXAMPLE_DATA	0x43, 0x01, 0xa4, 0xf2, 0x97, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x0b, 0x4b, 0xc3, 0x84, 0x53, 0x45, 0x4b, 0x55, 0x43, 0x48, 0x45, 0x4e, 0xc1, 0x00
#define EXAMPLE_DATA_LEN	26
         uint8_t example_data[ EXAMPLE_DATA_LEN ] = { EXAMPLE_DATA };
         wiselib::CoapPacket<Os> testpacket(example_data, EXAMPLE_DATA_LEN);
         debug_->debug( "set options are 0x%x\n ", testpacket.what_options_are_set() );
         size_t example_data_expected_length = testpacket.serialize_length();
         uint8_t example_data_reserialized[50];
         size_t example_data_length;
         example_data_length = testpacket.serialize( example_data_reserialized );
         if( example_data_expected_length != example_data_length )
         {
        	 debug_->debug( "serialize_length has bugs!" );
         }

         if( example_data_length < EXAMPLE_DATA_LEN )
         {
        	 debug_->debug( "reserialized length is smaller than input length (%i vs %i)\n", example_data_length, EXAMPLE_DATA_LEN );
        	 for(size_t i = 0; i < example_data_length; ++i )
        	 {
        		 if( example_data[i] != example_data_reserialized[i] )
        		 {
        			 debug_->debug( "reserialized data differs from input data at position %i ( %i vs %i )\n", i, example_data_reserialized[i], example_data[i]);
        		 }
        	 }
         }
         else
         {
        	 if( example_data_length > EXAMPLE_DATA_LEN )
        	 {
        		 debug_->debug( "reserialized length is bigger than input length\n" );
        	 }
        	 for(size_t i = 0; i < EXAMPLE_DATA_LEN; ++i )
        	 {
        		 if( example_data[i] != example_data_reserialized[i] )
        		 {
        			 debug_->debug( "reserialized data differs from input data at position %i ( %i vs %i )\n", i, example_data_reserialized[i], example_data[i]);
        		 }
        	 }
         }

         wiselib::CoapPacket<Os> testpacket2;
         debug_->debug( "setting type\n");
         testpacket2.set_type( COAP_MSG_TYPE_NON );
         debug_->debug( "setting msg id\n");
         testpacket2.set_msg_id( 0xbeef );
         debug_->debug( "setting code\n");
         testpacket2.set_code( COAP_CODE_GET );
         debug_->debug( "setting uri host\n");
         testpacket2.set_option(COAP_OPT_URI_HOST, wiselib::StaticString("CoapHost"));
         debug_->debug( "setting if_none_match\n");
         testpacket2.set_opt_if_none_match(true);

         debug_->debug( "setting stuff worked\n");

         uint8_t expected_result[15] = {
        		 // Version 1, Type NON, OC= 2;Code: GET; Msg ID 0xbeef
        		 0x52, 0x01, 0xbe, 0xef,
        		// Delta 5, length 8; C, o, a, p, H, o, s, t
        		 0x58, 0x43, 0x6f, 0x61, 0x70, 0x48, 0x6f, 0x73, 0x74,
        		 // fencepost
        		 0x90,
        		 // if_none_match
        		 0x70
         };

         uint8_t actual_result[20];
         uint actual_length = 0;
         actual_length = testpacket2.serialize( actual_result );

         if( actual_length != 15 )
         {
        	 debug_->debug( "actual_length is %i \n", actual_length);
         }

         for(size_t i = 0; i < actual_length; ++i )
         {
        	 if( actual_result[i] != expected_result[i] )
        	 {
        		 debug_->debug( "actual differs from expected at position %i ( %i vs %i )\n", i, actual_result[i], expected_result[i]);
        	 }
         }

         timer_->set_timer<ExampleApplication,
                           &ExampleApplication::broadcast_loop>( 5000, this, 0 );
      }
      // --------------------------------------------------------------------
      void broadcast_loop( void* )
      {
         debug_->debug( "broadcast_loop\n" );
//         Os::Radio::block_data_t message[] = "hello world!\0";
//         radio_->send( Os::Radio::BROADCAST_ADDRESS, sizeof(message), message );
//
//         // following can be used for periodic messages to sink
//         timer_->set_timer<ExampleApplication,
//                           &ExampleApplication::broadcast_loop>( 5000, this, 0 );
      }
      // --------------------------------------------------------------------
      void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
      {
//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
//         debug_->debug( "message is %s\n", buf );
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
