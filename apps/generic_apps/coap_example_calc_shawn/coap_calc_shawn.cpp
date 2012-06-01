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

	typedef wiselib::CoapServiceStatic<Os, Os::Radio, Os::Timer, Os::Rand, wiselib::StaticString> coap_service_t;
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
		cservice_.init( *radio_, *timer_, *rand_ );
		cservice_.enable_radio();

		server_id_ = 0;
		math = wiselib::StaticString("calculator");
		//
		if( radio_->id() == server_id_ )
		{
			debug_->debug( "node %x > Calculator CoAP Service booting.\n", radio_->id() );
			cservice_.reg_resource_callback< ExampleApplication, &ExampleApplication::receive_coap_server>( math, this );
		}
		else
		{
			num1 = (*rand_)(1000);
			num2 = (*rand_)(1000);
			op = '+';
			debug_->debug( "node %x > Starting 'calculator' client\n", radio_->id() );
			timer_->set_timer<ExampleApplication,
					&ExampleApplication::broadcast_loop>( 1000, this, 0 );
		}
	}

	// --------------------------------------------------------------------
	void broadcast_loop( void* counter )
	{
		wiselib::StaticString query("");
		char uri_query[64];
		sprintf( uri_query, "num1=%i&num2=%i&op=%c", num1, num2, op );
		num1 = (*rand_)(1000);
		num2 = (*rand_)(1000);
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

		debug_->debug( "node %x -- broadcast_loop> Sending request for %i %c %i\n", radio_->id(), num1, op, num2 );

		cservice_.get< ExampleApplication, &ExampleApplication::receive_coap_client>( server_id_, math, uri_query, this );
		timer_->set_timer<ExampleApplication,
		&ExampleApplication::broadcast_loop>( 1000, this, NULL );
	}
	// --------------------------------------------------------------------
	void receive_coap_client( received_message_t & message )
	{
		coap_packet_t packet = message.message();
		if( packet.is_response() )
		{
			block_data_t *data = packet.data();
			debug_->debug( "node %i -- receive_coap> Result: %s\n", radio_->id(), (char*) data );
		}
	}

	void receive_coap_server( received_message_t & message )
	{
		coap_packet_t packet = message.message();
		if( packet.is_request() && packet.uri_path() == math )
		{
			wiselib::list_static<Os, wiselib::StaticString, 4> query;
			packet.get_query_list( COAP_OPT_URI_QUERY, query );
			received_num1 = 0;
			received_num2 = 0;
			received_op = '+';

			wiselib::list_static<Os, wiselib::StaticString, 10>::iterator qit = query.begin();
			for( ; qit != query.end(); ++qit )
			{
				if( strstr( (*qit).c_str(), "num1=" ) == (*qit).c_str() )
				{
					received_num1 = (int32_t) atoi( (*qit).c_str() + 5 );
				}
				if( strstr( (*qit).c_str(), "num2=" ) == (*qit).c_str() )
				{
					received_num2 = (int32_t) atoi( (*qit).c_str() + 5 );
				}
				if( strstr( (*qit).c_str(), "op=" ) == (*qit).c_str() )
				{
					received_op = (*qit).c_str()[3];
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
				// clearly, division by zero results in 42!
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
//			debug_->debug( "%i %c %i = %i",
//					received_num1, received_op, received_num2, result );
			char reply[50];
			int len = sprintf( reply, "%i %c %i = %i",
					received_num1, received_op, received_num2, result );
			cservice_.reply( message, (block_data_t*) reply, len + 1 );
		}
	}
private:
	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;

	coap_service_t cservice_;
	node_id_t server_id_;

	wiselib::StaticString math;
	int32_t received_num1;
	int32_t received_num2;
	char received_op;

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
