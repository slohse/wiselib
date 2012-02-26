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

BOOST_FIXTURE_TEST_CASE( ConstructorsDestructors, FacetsFixture )
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


