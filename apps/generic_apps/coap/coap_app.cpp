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

//#define COAP_APP_DEBUG


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
   public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;

      void init( Os::AppMainParameter& value )
      {
         radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
         timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
         debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
//
#ifdef COAP_APP_DEBUG
         debug_->debug( "Example Application booting!\n" );
         debug_->debug( "Node %i present\n", radio_->id() );
#endif
         plusone =  wiselib::StaticString("plusone");
         math = wiselib::StaticString("really/long/path/that/does/math");
         testnumber = 0;
         num1 = 17;
         num2 = 13;
         op = '+';
//
//         radio_->reg_recv_callback<ExampleApplication,
//                                   &ExampleApplication::receive_radio_message>( this );
#define EXAMPLE_DATA	0x43, 0x01, 0xa4, 0xf2, 0x97, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x0b, 0x4b, 0xc3, 0x84, 0x53, 0x45, 0x4b, 0x55, 0x43, 0x48, 0x45, 0x4e, 0xc1, 0x00
#define EXAMPLE_DATA_LEN	26
         uint8_t example_data[ EXAMPLE_DATA_LEN ] = { EXAMPLE_DATA };
         wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString> testpacket;
         testpacket.parse_message(example_data, EXAMPLE_DATA_LEN);
#ifdef COAP_APP_DEBUG
         debug_->debug( "set options are 0x%x\n ", testpacket.what_options_are_set() );
#endif
         uint8_t example_data_reserialized[50];
         size_t example_data_length;
         example_data_length = testpacket.serialize( example_data_reserialized );
#ifdef COAP_APP_DEBUG
         size_t example_data_expected_length = testpacket.serialize_length();
         if( example_data_expected_length != example_data_length )
         {
        	 debug_->debug( "serialize_length has bugs!" );
         }
#endif
#ifdef COAP_APP_DEBUG
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
#endif
         wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString> testpacket2;
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting type\n");
#endif
         testpacket2.set_type( COAP_MSG_TYPE_NON );
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting msg id\n");
#endif
         testpacket2.set_msg_id( 0xbeef );
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting code\n");
#endif
         testpacket2.set_code( COAP_CODE_GET );
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting uri host\n");
#endif
         wiselib::StaticString coaphost("CoapHost");
         testpacket2.set_option(COAP_OPT_URI_HOST, coaphost);
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting if_none_match\n");
#endif
         testpacket2.set_opt_if_none_match(true);
#ifdef COAP_APP_DEBUG
         debug_->debug( "setting stuff worked\n");

         debug_->debug( "getting type: %i\n", testpacket2.type() );
         debug_->debug( "getting msg id: %i\n", testpacket2.msg_id());
         debug_->debug( "getting code: %i\n", testpacket2.code());
#endif
         wiselib::StaticString tp2_uri_host;
         testpacket2.get_option(COAP_OPT_URI_HOST, tp2_uri_host);
#ifdef COAP_APP_DEBUG
         debug_->debug( "getting uri host: %s\n", tp2_uri_host.c_str() );
         debug_->debug( "getting if_none_match: %i\n", testpacket2.opt_if_none_match() );
#endif
         wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString> testpacket_copy;
         testpacket_copy = testpacket2;
#ifdef COAP_APP_DEBUG
         debug_->debug( "getting type: %i\n", testpacket_copy.type() );
         debug_->debug( "getting msg id: %i\n", testpacket_copy.msg_id());
         debug_->debug( "getting code: %i\n", testpacket_copy.code());
#endif
         wiselib::StaticString tp2_copy_uri_host;
         testpacket2.get_option(COAP_OPT_URI_HOST, tp2_copy_uri_host);
#ifdef COAP_APP_DEBUG
         debug_->debug( "getting uri host: %s\n", tp2_copy_uri_host.c_str() );
         debug_->debug( "getting if_none_match: %i\n", testpacket_copy.opt_if_none_match() );
#endif

#ifdef COAP_APP_DEBUG
         uint8_t expected_result[15] = {
        		 // Version 1, Type NON, OC= 3;Code: GET; Msg ID 0xbeef
        		 0x53, 0x01, 0xbe, 0xef,
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
#endif
         cradio_.init( *radio_, *timer_, *debug_,  wiselib::FacetProvider<Os, Os::Rand>::get_facet(value));

         cradio_.enable_radio();

         if ( radio_->id() == 1 )
         {
        	 int coap_resource_id = cradio_.reg_resource_callback< ExampleApplication, &ExampleApplication::receive_coap>( plusone, this );
#ifdef COAP_APP_DEBUG
        	 debug_->debug( "Node %i -- ExampleApp::init()> registered resource at index %i\n", radio_->id(), coap_resource_id );
#endif
        	 coap_resource_id = cradio_.reg_resource_callback< ExampleApplication, &ExampleApplication::receive_coap>( math, this );
#ifdef COAP_APP_DEBUG
        	 debug_->debug( "Node %i -- ExampleApp::init()> registered resource at index %i\n", radio_->id(), coap_resource_id );
#endif
         }

         timer_->set_timer<ExampleApplication,
                           &ExampleApplication::broadcast_loop>( 1000, this, 0 );
      }
      // --------------------------------------------------------------------
      void broadcast_loop( void* )
      {
#ifdef COAP_APP_DEBUG
         debug_->debug( "broadcast_loop> Node %i\n", radio_->id() );
#endif
         if( radio_->id() == 0 )
         {
        	 wiselib::StaticString query("");
#ifdef COAP_APP_DEBUG
        	 debug_->debug( "Node %i -- ExampleApp::broadcast_loop()> sending request for plusone with number %i\n", radio_->id(), testnumber );
#endif
        	 cradio_.post< ExampleApplication, &ExampleApplication::receive_coap>( 1, plusone, query, this, &testnumber, 1 );
        	 testnumber += 2;

#ifdef SHAWN
        	 char uri_query[64];
        	 sprintf( uri_query, "num1=%i&num2=%i&op=%c", num1, num2, op );
        	 num1 += 7;
        	 switch(op)
        	 {
        	 case '+':
        		 op = '-';
        		 break;
        	 case '-':
        		 op = '*';
        		 break;
        	 case '*':
        		 op = '/';
        		 break;
        	 default:
        		 op = '+';
        		 break;
        	 }

        	 query = wiselib::StaticString(uri_query);
#ifdef COAP_APP_DEBUG
        	 debug_->debug( "Node %i -- ExampleApp::broadcast_loop()> requesting math with query %s\n", radio_->id(), uri_query );
#endif
        	 cradio_.get< ExampleApplication, &ExampleApplication::receive_coap>( 1, math, query, this, NULL, 0 );
#endif // SHAWN
         }
//         Os::Radio::block_data_t message[] = "hello world!\0";
//         radio_->send( Os::Radio::BROADCAST_ADDRESS, sizeof(message), message );
//
//         // following can be used for periodic messages to sink
         timer_->set_timer<ExampleApplication,
                           &ExampleApplication::broadcast_loop>( 1000, this, 0 );
      }
      // --------------------------------------------------------------------
      void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
      {
//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
//         debug_->debug( "message is %s\n", buf );
      }

      void receive_coap( Os::Radio::node_id_t from, wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>::ReceivedMessage & message )
      {
    	  wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_r packet = message.message();
#ifdef COAP_APP_DEBUG
    	  debug_->debug( "Node %i -- ExampleApp::receive_coap> received from node %i\n", radio_->id(), from );
#endif
    	  if( radio_->id() == 1 )
    	  {
    		  if( packet.is_request() && packet.uri_path() == plusone )
    		  {
#ifdef COAP_APP_DEBUG
    			  debug_->debug( "Node %i -- ExampleApp::receive_coap> request for resource %s\n", radio_->id(), plusone.c_str() );
#endif
    			  if( packet.data_length() > 0 )
    			  {
#ifdef COAP_APP_DEBUG
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> there is data\n", radio_->id() );
#endif
    				  uint8_t result = packet.data()[0] + 1;
    				  cradio_.reply( message, &result, 1 );
    			  }
    		  }

#ifdef SHAWN
    		  if( packet.is_request() && packet.uri_path() == math )
    		  {
#ifdef COAP_APP_DEBUG
    			  debug_->debug( "Node %i -- ExampleApp::receive_coap> request for resource %s\n", radio_->id(), math.c_str() );
#endif
    			  wiselib::list_static<Os, wiselib::StaticString, 10> query;
    			  packet.get_options( COAP_OPT_URI_QUERY, query );
    			  int32_t received_num1 = 0;
    			  int32_t received_num2 = 0;
    			  char received_op = '+';
    			  wiselib::list_static<Os, wiselib::StaticString, 10>::iterator qit = query.begin();
    			  for( ; qit != query.end(); ++qit )
    			  {
#ifdef COAP_APP_DEBUG
    			  debug_->debug( "Node %i -- ExampleApp::receive_coap> query %s\n", radio_->id(), (*qit).c_str() );
#endif
    				  if( strstr( (*qit).c_str(), "num1=" ) != NULL )
    				  {
    					  received_num1 = (int32_t) strtol( (*qit).c_str() + 5 , NULL, 0);
#ifdef COAP_APP_DEBUG
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> received_num1=%i\n", radio_->id(), received_num1 );
#endif
    				  }
    				  if( strstr( (*qit).c_str(), "num2=" ) != NULL )
    				  {
    					  received_num2 = (int32_t) strtol( (*qit).c_str() + 5 , NULL, 0);
#ifdef COAP_APP_DEBUG
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> received_num1=%i\n", radio_->id(), received_num2 );
#endif
    				  }
    				  if( strstr( (*qit).c_str(), "op=" ) != NULL )
    				  {
    					  received_op = (*qit).c_str()[3];
#ifdef COAP_APP_DEBUG
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> received_op=%c\n", radio_->id(), received_op );
#endif
    				  }
    			  }
    			  int32_t result = 0;
    			  switch( received_op )
    			  {
    			  case '+':
    				  result = received_num1 + received_num2;
    				  break;
    			  case '-':
    				  result = received_num1 - received_num2;
    				  break;
    			  case '/':
    				  // clearly division by zero results in 42!
    				  if(received_num2 == 0)
    					  result = 42;
    				  else
    					  result = received_num1 / received_num2;
    				  break;
    			  case '*':
    				  result = received_num1 * received_num2;
    				  break;
    			  default:
    				  break;
    				  // nop
    			  }
#ifdef COAP_APP_DEBUG
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> %i %c %i = %i\n"
    						  , radio_->id(), received_num1, received_op, received_num2, result );
#endif
    			  block_data_t result_ser[4];
    			  wiselib::write<Os , block_data_t , int32_t >( result_ser, result );
    			  cradio_.reply( message, result_ser, 4 );

    		  }
#endif // SHAWN
    	  }

    	  if( radio_->id() == 0 )
    	  {
    		  if( packet.is_response() )
    		  {
#ifdef COAP_APP_DEBUG

    			  if( packet.data_length() == 4 )
    			  {
    				  block_data_t *data = packet.data();
    				  int32_t math_result = wiselib::read<Os , block_data_t , int32_t >( data );
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> Result: %i\n", radio_->id(), math_result );
    				  num2 = math_result;
    			  }
    			  else
    			  {
    				  debug_->debug( "Node %i -- ExampleApp::receive_coap> received reply, code %i.%i, Payload length %i, Payload: "
    				      			, radio_->id(), ( ( packet.code() & 0xE0 ) >> 5 ), ( packet.code() & 0x1F ), packet.data_length() );
    				  for( size_t i = 0; i < packet.data_length(); ++i )
    				  {
    					  debug_->debug( "%i ", packet.data()[i] );
    				  }
    				  debug_->debug( "\n" );
    			  }
#endif
    		  }
    	  }
      }
   private:
      Os::Radio::self_pointer_t radio_;
      Os::Timer::self_pointer_t timer_;
      Os::Debug::self_pointer_t debug_;
      wiselib::StaticString plusone;
      wiselib::StaticString math;
      uint8_t testnumber;
      int32_t num1;
      int32_t num2;
      char op;

      wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> cradio_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
  example_app.init( value );
}
