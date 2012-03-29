#define BOOST_TEST_MODULE CoapTest
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
	cout << "serialize passed \n";
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_actual, packet_actual + packet_serialize_length_expected,
				packet_expected, packet_expected + packet_serialize_length_expected );

	cout << "checking serialized packet passed \n";
	packet.set_opt_if_none_match( false );
	cout << "setting opt_if_none_match false passed \n";

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

	packet.set_uri_path( slash_uri );

	block_data_t packet_expected[ ] = { 0x55, COAP_CODE_EMPTY, 0x00, 0x00 ,
	                    // uri_path, length 9, "uri_with_slash"
	                    0x9E, 'u', 'r', 'i', '_', 'w', 'i', 't', 'h', '_', 's', 'l', 'a', 's', 'h',
	                    // uri_path, length 2, "at"
	                    0x02, 'a', 't',
	                    0x09, 'b', 'e', 'g', 'i', 'n', 'n', 'i', 'n', 'g',
	                    0x03, 'a', 'n', 'd',
	                    0x03, 'e', 'n', 'd'
	                    };

	packet_serialize_length_expected = 40;

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

	packet.set_uri_path( fifteen_uri );

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


BOOST_AUTO_TEST_SUITE_END()
