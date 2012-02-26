#define BOOST_TEST_MODULE CoapPacketTest
#include <boost/test/included/unit_test.hpp>

#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_timer.h"

#include "util/pstl/static_string.h"
#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"
#include "unit_test_radio.h"


using namespace wiselib;

typedef PCOsModel Os;
typedef wiselib::StaticString string_t;
typedef CoapPacket<Os, UnitTestRadio, string_t>::coap_packet_t coap_packet_t;
typedef UnitTestRadio::block_data_t block_data_t;

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
		timer_ = new Os::Timer();
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
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;
};

BOOST_FIXTURE_TEST_CASE( Constructor, FacetsFixture )
{
	coap_packet_t packet;
	BOOST_CHECK_EQUAL( packet.code(), COAP_CODE_EMPTY );
	BOOST_CHECK_EQUAL( packet.version(), COAP_VERSION );
	BOOST_CHECK_EQUAL( packet.type(), COAP_MSG_TYPE_NON );
	BOOST_CHECK_EQUAL( packet.data_length(), 0 );
	BOOST_CHECK_EQUAL( packet.opt_if_none_match(), false );
	BOOST_CHECK_EQUAL( packet.what_options_are_set(), 0 );
	BOOST_CHECK_EQUAL( packet.uri_port(), COAP_STD_PORT );
}

BOOST_FIXTURE_TEST_CASE( Serialize, FacetsFixture )
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

BOOST_FIXTURE_TEST_CASE( Serialize2, FacetsFixture )
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


