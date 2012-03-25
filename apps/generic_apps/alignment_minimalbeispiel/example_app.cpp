/*
 * Simple Wiselib Example
 */
#include "external_interface/external_interface.h"
#include "algorithms/routing/tree/tree_routing.h"

#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

typedef wiselib::OSMODEL Os;

class ExampleApplication
{
public:
	typedef Os::Radio::block_data_t block_data_t;
	typedef wiselib::CoapPacket<Os, Os::Radio, wiselib::StaticString> coap_packet_t;
	typedef coap_packet_t::CoapOption<wiselib::StaticString> stringoption_t;

	void check_options_equal()
	{
		if( coap_str_option_ == other_coap_str_option_ )
		{
			debug_->debug( "Options are the same!\n" );
		}
		else
		{
			debug_->debug( "Options are different!\n" );
		}
	}

	void print_option(stringoption_t opt)
	{
		debug_->debug( "Option %i: %s\n", opt.option_number(), const_cast<wiselib::StaticString&>(opt.value()).c_str() );
	}

	void print_block_data( block_data_t *stream, size_t len )
	{
		if( len > 0 )
		{
			debug_->debug( "Block data: ");
			for( size_t i = 0; i < len; ++i )
			{
				debug_->debug( "%x ", stream[i] );
			}
			debug_->debug( "\n" );
		}
	}

	void init( Os::AppMainParameter& value )
	{
		radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet( value );
		timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet( value );
		debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet( value );

		debug_->debug( "Example Application booting!\n" );

		somestring_ = wiselib::StaticString("this string has super cow powers");
		other_string_ = wiselib::StaticString("hi friend!");

		coap_str_option_ = stringoption_t(17, somestring_ );
		other_coap_str_option_ = stringoption_t(23, other_string_ );

		check_options_equal();

		somestring_ = wiselib::StaticString("Moo!");

		other_coap_str_option_ = stringoption_t( 17, somestring_ );

		check_options_equal();

		coap_str_option_ = stringoption_t(17, somestring_ );

		check_options_equal();

		wiselib::list_static<Os, stringoption_t, 8> string_options_;

		string_options_.push_back(coap_str_option_);
		string_options_.push_back(other_coap_str_option_);

		stringoption_t retrieved = string_options_.back();

		print_option( retrieved );

		string_options_.clear();

		coap_str_option_ = stringoption_t(23, other_string_ );

		string_options_.push_back(other_coap_str_option_);
		string_options_.push_back(coap_str_option_);
		retrieved = string_options_.back();
		print_option( retrieved );

		coap_packet_t packet;
		size_t full_opaque = 8;
		uint8_t tkn_array[ ] = { 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x23, 0x45 };
		wiselib::OpaqueData tkn( tkn_array, full_opaque );
		packet.set_token(tkn);

		somestring_ = wiselib::StaticString("some/path");
		packet.set_uri_path( somestring_ );

//		block_data_t serial[30];

//		size_t serial_len = packet.serialize(serial);

//		print_block_data( serial, serial_len );

		debug_->debug( "sizeof size_t: %i\n", sizeof(size_t) );
		debug_->debug( "sizeof pointer: %i\n", sizeof(void*) );
		debug_->debug( "sizeof OpaqueData: %i\n", sizeof(wiselib::OpaqueData) );
		debug_->debug( "sizeof CoapOption<OpaqueData>: %i\n", sizeof(coap_packet_t::CoapOption<wiselib::OpaqueData>) );
		debug_->debug( "sizeof StaticString: %i\n", sizeof(wiselib::StaticString) );
		debug_->debug( "sizeof CoapOption<StaticString>: %i\n", sizeof(stringoption_t) );
		debug_->debug( "sizeof uint32_t: %i\n", sizeof(uint32_t) );
		debug_->debug( "sizeof CoapOption<uint32_t>: %i\n", sizeof(coap_packet_t::CoapOption<uint32_t>) );
		debug_->debug( "sizeof string_list: %i\n", sizeof(wiselib::list_static<Os, stringoption_t, COAP_LIST_SIZE_STRING>) );
		debug_->debug( "sizeof opaque_list: %i\n", sizeof(wiselib::list_static<Os, coap_packet_t::CoapOption<wiselib::OpaqueData>, COAP_LIST_SIZE_OPAQUE>) );
		debug_->debug( "sizeof uint_list: %i\n", sizeof(wiselib::list_static<Os, coap_packet_t::CoapOption<uint32_t>, COAP_LIST_SIZE_UINT>) );
		debug_->debug( "sizeof uint_list Groesse 1: %i\n", sizeof(wiselib::list_static<Os, coap_packet_t::CoapOption<uint32_t>, 1>) );
		debug_->debug( "sizeof uint_list Groesse 2: %i\n", sizeof(wiselib::list_static<Os, coap_packet_t::CoapOption<uint32_t>, 2>) );
		debug_->debug( "sizeof uint_list Groesse 3: %i\n", sizeof(wiselib::list_static<Os, coap_packet_t::CoapOption<uint32_t>, 3>) );
		debug_->debug( "sizeof coap_packet_t: %i\n", sizeof(coap_packet_t) );
		debug_->debug( "sizeof CoapRadio: %i\n", sizeof(wiselib::CoapRadio<Os, Os::Radio, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString>));

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
	stringoption_t coap_str_option_;
	stringoption_t other_coap_str_option_;

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------
void application_main( Os::AppMainParameter& value )
{
	example_app.init( value );
}
