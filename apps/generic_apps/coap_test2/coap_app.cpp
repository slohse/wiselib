/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "util/pstl/static_string.h"
#include "util/pstl/string_dynamic.h"

//#include "util/allocators/malloc_free_allocator.h"

#include "stdlib.h"

#define COAP_APP_DEBUG


typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:

	typedef Os::Radio::node_id_t node_id_t;
	typedef Os::Radio::block_data_t block_data_t;

	void init( Os::AppMainParameter& value )
	{
#ifdef COAP_APP_DEBUG
		debug_->debug( "Example Application booting!\n" );
#endif
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );
		//

#ifdef COAP_APP_DEBUG
		debug_->debug( "Node %i present\n", radio_->id() );
#endif

		cradio_.init( *radio_, *timer_, *debug_,  wiselib::FacetProvider<Os, Os::Rand>::get_facet(value));
		math = wiselib::StaticString("really/long/path/that/does/math");
		num1 = 17;
		num2 = 13;
		op = '+';

		cradio_.enable_radio();

		if ( radio_->id() == 1 )
		{
			int coap_resource_id = cradio_.reg_resource_callback< ExampleApplication, &ExampleApplication::receive_coap>( math, this );
#ifdef COAP_APP_DEBUG
			debug_->debug( "Node %i -- ExampleApp::init()> registered resource '%s' at index %i\n", radio_->id(), math.c_str(), coap_resource_id );
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
			wiselib::StaticString error_request( "this/resource/doesnt/exist" );
			cradio_.get< ExampleApplication, &ExampleApplication::receive_coap>( 1, error_request, query, this, NULL, 0 );

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
		timer_->set_timer<ExampleApplication,
		&ExampleApplication::broadcast_loop>( 1000, this, 0 );
	}
	// --------------------------------------------------------------------
	void receive_radio_message( Os::Radio::node_id_t from, Os::Radio::size_t len, Os::Radio::block_data_t *buf )
	{
		//         debug_->debug( "received msg at %x from %x\n", radio_->id(), from );
		//         debug_->debug( "message is %s\n", buf );
	}

	void receive_coap( Os::Radio::node_id_t from, const wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString>::coap_packet_r packet )
	{
#ifdef COAP_APP_DEBUG
		debug_->debug( "Node %i -- ExampleApp::receive_coap> received from node %i\n", radio_->id(), from );
#endif
		if( radio_->id() == 1 )
		{
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
#ifdef COAP_APP_DEBUG
					debug_->debug( "Node %i -- ExampleApp::receive_coap> query: ", radio_->id() );
#endif
				for( ; qit != query.end(); ++qit )
				{
					if( strstr( (*qit).c_str(), "num1=" ) != NULL )
					{
						received_num1 = (int32_t) strtol( (*qit).c_str() + 5 , NULL, 0);
#ifdef COAP_APP_DEBUG
						debug_->debug( "_num1=%i, ", received_num1 );
#endif
					}
					if( strstr( (*qit).c_str(), "num2=" ) != NULL )
					{
						received_num2 = (int32_t) strtol( (*qit).c_str() + 5 , NULL, 0);
#ifdef COAP_APP_DEBUG
						debug_->debug( "num2=%i, ", received_num2 );
#endif
					}
					if( strstr( (*qit).c_str(), "op=" ) != NULL )
					{
						received_op = (*qit).c_str()[3];
#ifdef COAP_APP_DEBUG
						debug_->debug( "op=%c, ", received_op );
#endif
					}
				}
#ifdef COAP_APP_DEBUG
					debug_->debug( "\n" );
#endif
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
				cradio_.reply( packet, result_ser, 4 );

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
					char payload_string[ packet.data_length() + 1 ];
					memcpy( payload_string, packet.data(), packet.data_length() );
					payload_string[ packet.data_length()] = '\0';
/*					for( size_t i = 0; i < packet.data_length(); ++i )
					{
						debug_->debug( "%i ", packet.data()[i] );
					}
*/					debug_->debug( "%s\n", payload_string );
				}
#endif
			}
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;

	wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> cradio_;
	wiselib::StaticString math;
	int32_t num1;
	int32_t num2;
	char op;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
