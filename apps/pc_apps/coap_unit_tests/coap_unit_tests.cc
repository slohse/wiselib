#define BOOST_TEST_MODULE CoapTest
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <list>

#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_timer.h"

#include "util/pstl/static_string.h"
#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"
#include "unit_test_radio.h"
#include "timer_stub.h"


using namespace wiselib;

typedef PCOsModel Os;
typedef wiselib::StaticString string_t;
typedef CoapPacket<Os, UnitTestRadio, string_t>::coap_packet_t coap_packet_t;
typedef CoapRadio<Os, UnitTestRadio, DummyTimerModel, Os::Debug, Os::Rand, string_t> coapradio_t;
typedef UnitTestRadio::block_data_t block_data_t;
typedef UnitTestRadio::node_id_t node_id_t;

typedef coapradio_t::TimerAction TimerAction;

typedef delegate1<void, void*> timer_delegate_t;

//changing output of chars (block_data_t) to numbers
namespace std {
ostream &operator<<( ostream &os, const unsigned char &uc )
{
	return os << static_cast<unsigned>(uc);
}
}

class DummyResource
{
public:
	DummyResource()
	{

	}

	~DummyResource()
	{

	}

	void receive_coap( coapradio_t::ReceivedMessage & message )
	{
		messages_.push_back(&message);
	}

	list<coapradio_t::ReceivedMessage*> messages_;

};

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
	BOOST_CHECK_EQUAL( packet.what_options_are_set(), 0 );
	BOOST_CHECK_EQUAL( packet.uri_port(), COAP_STD_PORT );
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

	string expected_path( "storage/KAESEKUCHEN" );
	string actual_path( packet.uri_path().c_str() );

	BOOST_CHECK_EQUAL( expected_path, actual_path );

	BOOST_CHECK( packet.opt_if_none_match() );

	block_data_t packet_reserialized[200];

	block_data_t packet_reserialized_expected[ ] = { 0x43, COAP_CODE_GET, 0xa4, 0xf2,
			// Uri-Path, length 7, "storage"
			0x97, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
			// Uri-Path, length 11, "KAESEKUCHEN"
			0x0b, 0x4b, 0x41, 0x45, 0x53, 0x45, 0x4b, 0x55, 0x43, 0x48, 0x45, 0x4e,
			// If-None-Match without unneccessary value Byte
			0xc0 };

	size_t packet_length_expected = 25;

	size_t packet_length_actual = packet.serialize( packet_reserialized );

	BOOST_CHECK_EQUAL( packet_length_expected, packet_length_actual );
	BOOST_CHECK_EQUAL_COLLECTIONS( packet_reserialized,
			packet_reserialized + packet_length_expected,
			packet_reserialized_expected,
			packet_reserialized_expected + packet_length_expected );

}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CoapRadio)

BOOST_FIXTURE_TEST_CASE( ACKschedule, FacetsFixture )
{
	size_t packet_length = 25;
	block_data_t packet[ ] =
			// Confirmable message
			{ 0x43, COAP_CODE_GET, 0xa4, 0xf2,
			// Uri-Path, length 7, "storage"
			0x97, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
			// Uri-Path, length 11, "KAESEKUCHEN"
			0x0b, 0x4b, 0x41, 0x45, 0x53, 0x45, 0x4b, 0x55, 0x43, 0x48, 0x45, 0x4e,
			// If-None-Match with one unneccessary value Byte
			0xc0 };

	BOOST_CHECK_EQUAL( radio_->sentMessages() , 0 );

	node_id_t id = 23;
	coapradio_t cradio;
	cradio.init( *radio_, *timer_, *debug_ , *rand_ );

	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 0 );

	cradio.receive( id, packet_length, packet );

	// a 4.04 should have been sent
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 1 );

	node_id_t id_actual;
	UnitTestRadio::size_t len;
	block_data_t *data;
	radio_->lastMessage().get( id_actual, len, data );

	block_data_t packet_expected[ ] =
			// piggybacked 4.04
			{ 0x60, COAP_CODE_NOT_FOUND, 0xa4, 0xf2 };

	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 4 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			packet_expected,
			packet_expected + len );

	// a timeout should have been scheduled to send an ACK to the received Message
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 1 );

	BOOST_CHECK_EQUAL( timer_->lastEvent().time_, COAP_ACK_GRACE_PERIOD );

	TimerAction taction = cradio.timers_.at( (size_t) timer_->lastEvent().userdata_ );

	BOOST_CHECK_EQUAL( taction.type_, TIMER_ACK );

	// cause a timeout on the ACK
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 1 );

	timer_->lastEvent().callback_( timer_->lastEvent().userdata_ );

	// no message should have been sent, as the piggybacked ACK already went out
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 1 );
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 1 );

	// request again. Same response should go out
	cradio.receive( id, packet_length, packet );
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 2 );
	radio_->lastMessage().get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 4 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			packet_expected,
			packet_expected + len );



}

BOOST_FIXTURE_TEST_CASE( ACKschedule2, FacetsFixture )
{
	node_id_t id = 23;
	coapradio_t cradio;
	cradio.init( *radio_, *timer_, *debug_ , *rand_ );

	DummyResource dresource = DummyResource();
	// register a resource
	cradio.reg_resource_callback<DummyResource, &DummyResource::receive_coap>( string_t("dummy"), &dresource );

	size_t dummy_length = 10;
	block_data_t dummy_request[ ] =
			// Confirmable GET, id 0xaffe
			{ 0x41, COAP_CODE_GET, 0xaf, 0xfe,
			// Uri-Path "dummy"
			0x95, 0x64, 0x75, 0x6d, 0x6d, 0x79
			};

	cradio.receive( id, dummy_length, dummy_request );
	// new ACK timeout should be present
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 1 );
	BOOST_CHECK_EQUAL( timer_->lastEvent().time_, COAP_ACK_GRACE_PERIOD );
	TimerAction taction = cradio.timers_.at( (size_t) timer_->lastEvent().userdata_ );
	BOOST_CHECK_EQUAL( taction.type_, TIMER_ACK );
	// no message should be sent, as the resource is present, but has not yet
	// returned a result
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 0 );
	// resources handler should have been called
	BOOST_CHECK_EQUAL( dresource.messages_.size(), 1);

	// ack grace period is over
	timer_->lastEvent().callback_( timer_->lastEvent().userdata_ );
	// ACK should have been sent
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 1 );

	block_data_t dummy_ack[ ] =
			// ACK, id 0xaffe
			{ 0x60, COAP_CODE_EMPTY, 0xaf, 0xfe };
	node_id_t id_actual;
	UnitTestRadio::size_t len;
	block_data_t *data;
	radio_->lastMessage().get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 4 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			dummy_ack,
			dummy_ack + len );

	// retransmit should lead to retransmit of ACK
	cradio.receive( id, dummy_length, dummy_request );
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 2 );
	radio_->lastMessage().get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 4 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			dummy_ack,
			dummy_ack + len );
	// the resources handler should not have been called again
	BOOST_CHECK_EQUAL( dresource.messages_.size(), 1);

	block_data_t dummy_reply[] = { 0x68, 0x69, 0x74, 0x68, 0x65, 0x72, 0x65 };
	size_t dummy_reply_len = 7;
	cradio.reply( *(dresource.messages_.back()), dummy_reply, dummy_reply_len );
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 3 );

	coap_msg_id_t random_msg_id = cradio.msg_id_ - 1;
	block_data_t dummy_reply_expected[] =
			// CON reply, 2.05, id random
			{ 0x40, COAP_CODE_CONTENT, (random_msg_id & 0xff00) >> 8, random_msg_id & 0xff,
			// payload
			0x68, 0x69, 0x74, 0x68, 0x65, 0x72, 0x65
			};
	radio_->lastMessage().get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 11 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			dummy_reply_expected,
			dummy_reply_expected + len );

	// retransmit timeout should be scheduled
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 2 );
	taction = cradio.timers_.at( (size_t) timer_->lastEvent().userdata_ );
	BOOST_CHECK_EQUAL( taction.type_, TIMER_RETRANSMIT );
	BOOST_CHECK_GE(timer_->lastEvent().time_, COAP_RESPONSE_TIMEOUT );
	BOOST_CHECK_LE(timer_->lastEvent().time_, COAP_MAX_RESPONSE_TIMEOUT );

	// cause timeout of retransmit
	timer_->lastEvent().callback_( timer_->lastEvent().userdata_ );
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 3 );
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 4 );

	// cause retransmits. No new timeouts, but two new messages (ACK and response)
	cradio.receive( id, dummy_length, dummy_request );
	BOOST_CHECK_EQUAL( timer_->scheduledEvents() , 3 );
	BOOST_CHECK_EQUAL( radio_->sentMessages() , 6 );
	// response
	radio_->lastMessage().get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 11 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			dummy_reply_expected,
			dummy_reply_expected + len );

	list<UnitTestRadio::Message>::iterator it = radio_->sent_.end();
	--it;
	--it;
	(*it).get( id_actual, len, data );
	BOOST_CHECK_EQUAL( id , id_actual );
	BOOST_CHECK_EQUAL( len , 4 );
	BOOST_CHECK_EQUAL_COLLECTIONS( data,
			data + len,
			dummy_ack,
			dummy_ack + len );
}

BOOST_AUTO_TEST_SUITE_END()


