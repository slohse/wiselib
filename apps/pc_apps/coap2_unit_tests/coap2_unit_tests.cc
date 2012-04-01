#define BOOST_TEST_MODULE CoapTest
#define VERBOSE_DEBUG
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <list>

#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_timer.h"

#include "util/pstl/static_string.h"
#include "radio/coap/coap_packet.h"
#include "unit_test_radio.h"
#include "timer_stub.h"


using namespace wiselib;



typedef PCOsModel Os;
typedef wiselib::StaticString string_t;
typedef CoapPacket<Os, UnitTestRadio, string_t>::coap_packet_t coap_packet_t;
typedef UnitTestRadio::block_data_t block_data_t;
typedef UnitTestRadio::node_id_t node_id_t;

typedef delegate1<void, void*> timer_delegate_t;

//changing output of chars (block_data_t) to numbers
namespace std {
ostream &operator<<( ostream &os, const unsigned char &uc )
{
	return os << static_cast<unsigned>(uc);
}
}


struct FacetsFixture {
	FacetsFixture()
	{
		debug_ = new Os::Debug();
		timer_ = new DummyTimerModel();
		rand_ =  new Os::Rand();
		radio_ = new UnitTestRadio();
	}
	
	~FacetsFixture()
	{
		delete debug_;
		delete timer_;
		delete rand_;
		delete radio_;
	}

	UnitTestRadio *radio_;
	DummyTimerModel *timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
};

BOOST_AUTO_TEST_SUITE(CoapPacket)

BOOST_FIXTURE_TEST_CASE( Constructor, FacetsFixture )
{
	coap_packet_t packet;

	BOOST_CHECK_EQUAL( packet.code(), COAP_CODE_EMPTY );
	BOOST_CHECK_EQUAL( packet.version(), COAP_VERSION );
	BOOST_CHECK_EQUAL( packet.type(), COAP_MSG_TYPE_NON );
	BOOST_CHECK_EQUAL( packet.data_length(), 0 );
	BOOST_CHECK_EQUAL( packet.opt_if_none_match(), false );
//	BOOST_CHECK_EQUAL( packet.what_options_are_set(), 0 );
//	BOOST_CHECK_EQUAL( packet.uri_port(), COAP_STD_PORT );
}

BOOST_FIXTURE_TEST_CASE( Serialize_minimal_packet, FacetsFixture )
{
	coap_packet_t packet;
	size_t vanilla_packet_serialize_length_expected = 4;
	//just the header
	BOOST_CHECK_EQUAL( packet.serialize_length(), vanilla_packet_serialize_length_expected );

	block_data_t vanilla_packet_expected[] = { 0x50, COAP_CODE_EMPTY, 0x00, 0x00 };
	block_data_t vanilla_packet_actual[ packet.serialize_length() ];
	size_t vanilla_packet_serialize_length_actual = packet.serialize( vanilla_packet_actual );
	BOOST_CHECK_EQUAL( vanilla_packet_serialize_length_actual , vanilla_packet_serialize_length_expected );
	BOOST_CHECK_EQUAL_COLLECTIONS( vanilla_packet_actual, vanilla_packet_actual + vanilla_packet_serialize_length_expected,
			vanilla_packet_expected, vanilla_packet_expected + vanilla_packet_serialize_length_expected );
}

BOOST_FIXTURE_TEST_CASE( Serialize_fence_posts, FacetsFixture )
{
	coap_packet_t packet;
	size_t packet_serialize_length_expected = 6;
	block_data_t packet_actual[ 200 ];

	packet.set_opt_if_none_match( true );
	BOOST_CHECK_EQUAL( packet.serialize_length(), packet_serialize_length_expected );

	block_data_t packet_expected[] = { 0x52, COAP_CODE_EMPTY, 0x00, 0x00,
			( ( COAP_OPT_FENCEPOST & 0x0F) << 4 ), ( ( 7 & 0x0F) << 4 ) };
	size_t packet_serialize_length_actual = packet.serialize( packet_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
				packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_opt_if_none_match( false );

	block_data_t vanilla_packet_expected[] = { 0x50, COAP_CODE_EMPTY, 0x00, 0x00 };

	packet_serialize_length_actual = packet.serialize( packet_actual );
	size_t vanilla_packet_serialize_length_expected = 4;

	BOOST_CHECK_EQUAL( packet.serialize_length(), packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + vanilla_packet_serialize_length_expected,
				vanilla_packet_expected, vanilla_packet_expected + vanilla_packet_serialize_length_expected );

}

BOOST_FIXTURE_TEST_CASE( Serialize_msg_id_token, FacetsFixture )
{
	coap_packet_t packet;
	block_data_t packet_actual[ 200 ];

	size_t packet_serialize_length_expected;
	size_t packet_serialize_length_actual;

	packet.set_msg_id( 0x1234 );
	size_t full_opaque = 8;
	uint8_t tkn_array[ ] = { 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x23, 0x45 };
	OpaqueData tkn( tkn_array, full_opaque );
	packet.set_token( tkn );

	OpaqueData tkncheck;

	// 4 Header, 1 option header, 8 token
	packet_serialize_length_expected = 13;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	block_data_t packet_expected[ ] = { 0x51, COAP_CODE_EMPTY, 0x12, 0x34, 0xB8, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x23, 0x45 };

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
					packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_token( tkncheck );

	packet.token( tkncheck );

	BOOST_CHECK( tkncheck == OpaqueData() );

	packet_serialize_length_expected = 4;
	block_data_t packet_expected2[ ] = { 0x50, COAP_CODE_EMPTY, 0x12, 0x34 };

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
					packet_expected2, packet_expected2 + packet_serialize_length_expected );

	uint8_t tkn_array2[ ] = { 0 };
	tkn.set( tkn_array2, 1 );
	packet.set_token( tkn );

	packet_serialize_length_expected = 6;
	block_data_t packet_expected3[ ] = { 0x51, COAP_CODE_EMPTY, 0x12, 0x34, 0xB1, 0x00 };

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
					packet_expected3, packet_expected3 + packet_serialize_length_expected );

}

BOOST_FIXTURE_TEST_CASE( Boundary_Condition_Options_string_delimiters, FacetsFixture )
{
	coap_packet_t packet;
	block_data_t packet_actual[ 200 ];

	size_t packet_serialize_length_expected;
	size_t packet_serialize_length_actual;

	string_t slash_uri = string_t("/uri_with_slash/at/beginning/and/end/");
	string_t uri_query = string_t("/?param1=7&op=+&muh=maeh");

	packet.set_uri_path( slash_uri );
	packet.set_uri_query( uri_query );

	block_data_t packet_expected[ ] = { 0x58, COAP_CODE_EMPTY, 0x00, 0x00 ,
	                    // uri_path, length 9, "uri_with_slash"
	                    0x9E, 'u', 'r', 'i', '_', 'w', 'i', 't', 'h', '_', 's', 'l', 'a', 's', 'h',
	                    // uri_path, length 2, "at"
	                    0x02, 'a', 't',
	                    0x09, 'b', 'e', 'g', 'i', 'n', 'n', 'i', 'n', 'g',
	                    0x03, 'a', 'n', 'd',
	                    0x03, 'e', 'n', 'd',
	                    0x68, 'p', 'a', 'r', 'a', 'm', '1', '=', '7',
	                    0x04, 'o', 'p', '=', '+',
	                    0x08, 'm', 'u', 'h', '=', 'm', 'a', 'e', 'h'
	                    };

	packet_serialize_length_expected = 63;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
					packet_expected, packet_expected + packet_serialize_length_expected );


}

BOOST_FIXTURE_TEST_CASE( Boundary_Condition_Options_string_length, FacetsFixture )
{
	coap_packet_t packet;
	block_data_t packet_actual[ 200 ];

	size_t packet_serialize_length_expected;
	size_t packet_serialize_length_actual;

	string_t fifteen_uri = string_t("uri_15_char_len/segment");

	BOOST_CHECK_EQUAL( packet.set_uri_path( fifteen_uri ), coap_packet_t::SUCCESS );

	// TODO Längencheck mit dynamischen String und Länge > 270

	block_data_t packet_expected[ ] = { 0x52, COAP_CODE_EMPTY, 0x00, 0x00 ,
	                    // uri_path, length 15, "uri_15_char_len"
	                    0x9F, 0x00, 'u', 'r', 'i', '_', '1', '5', '_', 'c', 'h', 'a', 'r', '_', 'l', 'e', 'n',
	                    0x07, 's', 'e', 'g', 'm', 'e', 'n', 't'
	                    };

	packet_serialize_length_expected = 29;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );
}

BOOST_FIXTURE_TEST_CASE( uint_options, FacetsFixture )
{
	coap_packet_t packet;
	block_data_t packet_actual[ 200 ];

	size_t packet_serialize_length_expected;
	size_t packet_serialize_length_actual;

	packet.set_option( COAP_OPT_MAX_AGE, 0x12345678 );

	block_data_t packet_expected[ ] = { 0x51, COAP_CODE_EMPTY, 0x00, 0x00,
	                    0x24, 0x12, 0x34, 0x56, 0x78 };
	packet_serialize_length_expected = 9;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_option( COAP_OPT_MAX_AGE, 0x12000000 );
	packet_expected[6] = 0;
	packet_expected[7] = 0;
	packet_expected[8] = 0;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_option( COAP_OPT_MAX_AGE, 0x800000 );
	packet_expected[4] = 0x23;
	packet_expected[5] = 0x80;
	packet_expected[6] = 0;
	packet_expected[7] = 0;
	packet_serialize_length_expected = 8;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_option( COAP_OPT_MAX_AGE, 0x0300 );
	packet_expected[4] = 0x22;
	packet_expected[5] = 0x03;
	packet_expected[6] = 0;
	packet_serialize_length_expected = 7;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_option( COAP_OPT_MAX_AGE, 0x05 );
	packet_expected[4] = 0x21;
	packet_expected[5] = 0x05;
	packet_serialize_length_expected = 6;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

	packet.set_option( COAP_OPT_MAX_AGE, 0 );
	packet_expected[4] = 0x20;
	packet_serialize_length_expected = 5;

	packet_serialize_length_actual = packet.serialize( packet_actual );

	BOOST_CHECK_EQUAL( packet_serialize_length_expected, packet_serialize_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
			packet_expected, packet_expected + packet_serialize_length_expected );

}

BOOST_FIXTURE_TEST_CASE( parse_and_uri_path, FacetsFixture )
{

	size_t packet_length = 26;

	block_data_t packet_stream[ ] = { 0x43, COAP_CODE_GET, 0xa4, 0xf2,
			// Uri-Path, length 7, "storage"
			0x97, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
			// Uri-Path, length 11, "KAESEKUCHEN"
			0x0b, 0x4b, 0x41, 0x45, 0x53, 0x45, 0x4b, 0x55, 0x43, 0x48, 0x45, 0x4e,
			// If-None-Match with one unneccessary value Byte
			0xc1, 0x00 };

	coap_packet_t packet;

	packet.parse_message( packet_stream, packet_length );
	cout << "parsing checkpoint\n";

	string expected_path( "storage/KAESEKUCHEN" );
	string actual_path( packet.uri_path().c_str() );
	cout << "uri_path checkpoint\n";

	BOOST_CHECK_EQUAL( expected_path, actual_path );

	BOOST_CHECK( packet.opt_if_none_match() );
	cout << "opt_if_none_match checkpoint\n";

}


BOOST_AUTO_TEST_SUITE_END()
