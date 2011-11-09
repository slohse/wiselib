/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"


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
