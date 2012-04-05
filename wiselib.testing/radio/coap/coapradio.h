#ifndef COAPRADIO_H
#define COAPRADIO_H

#include "coap.h"
#include "coap_packet.h"
#include "util/delegates/delegate.hpp"
#include "util/pstl/vector_static.h"
#include "string.h"

namespace wiselib {

template<typename OsModel_P,
	typename Radio_P = typename OsModel_P::Radio,
	typename Timer_P = typename OsModel_P::Timer,
	typename Debug_P = typename OsModel_P::Debug,
	typename Rand_P = typename OsModel_P::Rand,
	typename String_T = wiselib::StaticString>
	class CoapRadio
	{

	public:
		// Type definitions
		typedef OsModel_P OsModel;

		typedef Radio_P Radio;

		typedef Debug_P Debug;
		typedef Timer_P Timer;
		typedef Rand_P Rand;
		typedef String_T string_t;

		typedef typename Radio::node_id_t node_id_t;
		typedef typename Radio::size_t size_t;
		typedef typename Radio::block_data_t block_data_t;
		typedef typename Radio::message_id_t message_id_t;

		typedef CoapRadio<OsModel, Radio, Timer, Debug, Rand, string_t> self_type;
		typedef self_type* self_pointer_t;

		typedef CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T> self_t;
		typedef self_t CoapRadio_t;

		typedef typename CoapPacket<OsModel, Radio, string_t>::coap_packet_t coap_packet_t;

		class ReceivedMessage
		{
		public:

			ReceivedMessage& operator=( const ReceivedMessage &rhs )
			{
				// avoid self-assignment
				if(this != &rhs)
				{
					message_ = rhs.message_;
					correspondent_ = rhs.correspondent_;
					ack_ = rhs.ack_;
					response_ = rhs.response_;
				}
				return *this;
			}

			ReceivedMessage()
			{
				message_ = coap_packet_t();
				ack_ = NULL;
				response_ = false;
			}

			ReceivedMessage( const ReceivedMessage &rhs )
			{
				*this = rhs;
			}

			ReceivedMessage( const coap_packet_t &packet, node_id_t from )
			{
				message_ = packet;
				correspondent_ = from;
				ack_ = NULL;
				response_ = false;
			}

			coap_packet_t & message() const
			{
				return message_;
			}

			coap_packet_t & message()
			{
				return message_;
			}

			node_id_t correspondent() const
			{
				return correspondent_;
			}

			coap_packet_t * ack_sent() const
			{
				return ack_;
			}

			coap_packet_t * response_sent() const
			{
				return response_;
			}

		private:
			template<typename __os, typename __radio, typename __timer, typename __debug, typename __rand, typename __string> friend class CoapRadio;
			coap_packet_t message_;
			// in this case the sender
			node_id_t correspondent_;
			coap_packet_t *ack_;
			coap_packet_t *response_;
			// TODO: empfangszeit? (Freshness)

			void set_message( const coap_packet_t &message)
			{
				message_ = message;
			}

			void set_correspondent( node_id_t correspondent)
			{
				correspondent_ = correspondent;
			}

			void set_ack_sent( coap_packet_t *ack )
			{
				ack_ = ack;
			}

			void set_response_sent(coap_packet_t *response)
			{
				response_ = response;
			}
		};

		typedef delegate1<void, ReceivedMessage&> coapreceiver_delegate_t;

		CoapRadio();
		~CoapRadio();

		int init();
		int destruct();
		int init( Radio& radio, Timer& timer, Debug& debug , Rand& rand );
		int enable_radio();
		int disable_radio();
		node_id_t id ();
		int send (node_id_t receiver, size_t len, block_data_t *data );
		void receive(node_id_t from, size_t len, block_data_t * data);

		// TODO: comment!!
		/**
		 * Sends the Message passed AS IT IS. That means: no sanity checks, no message ID or token generation.
		 * Most likely this is NOT what you want, use TODO instead
		 * @param receiver node ID of the receiver
		 * @param message Coap Packet to send
		 * @param callback delegate for responses from the receiver
		 */
		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* send_coap_as_is(node_id_t receiver, const coap_packet_t & message, T *callback);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* send_coap_gen_msg_id(node_id_t receiver, coap_packet_t & message, T *callback);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* send_coap_gen_msg_id_token(node_id_t receiver, coap_packet_t & message, T *callback);
		
		coap_packet_t* rst( node_id_t receiver, coap_msg_id_t id );

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		int reg_resource_callback( string_t resource_path, T *callback );

		int unreg_resource_callback( int idx );

		void receive_coap(ReceivedMessage& message);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* get(node_id_t receiver,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* put(node_id_t receiver,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					uint8_t* payload = NULL,
					size_t payload_length = 0,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* post(node_id_t receiver,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					uint8_t* payload = NULL,
					size_t payload_length = 0,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* del(node_id_t receiver,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		template<class T, void (T::*TMethod)(ReceivedMessage&)>
		coap_packet_t* request(node_id_t receiver,
					CoapCode code,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					uint8_t* payload = NULL,
					size_t payload_length = 0,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		coap_packet_t* reply( ReceivedMessage& req_msg,
				uint8_t* payload,
				size_t payload_length,
				CoapCode code = COAP_CODE_CONTENT );

		enum error_codes
		{
			// inherited from concepts::BasicReturnValues_concept
			SUCCESS = OsModel::SUCCESS,
			ERR_UNSPEC = OsModel::ERR_UNSPEC,
			ERR_NOTIMPL = OsModel::ERR_NOTIMPL
		};

	private:
#ifdef BOOST_TEST_DECL
		// *cough* ugly hackery
		// this is probably frowned upon, but I'd like some insight into
		// private members when testing
	public:
#endif
		class SentMessage
		{
		public:
			SentMessage()
			{
				retransmit_count_ = 0;
				ack_received_ = false;
				sender_callback_ = coapreceiver_delegate_t();
				response_ = NULL;
			}

			coap_packet_t & message() const
			{
				return message_;
			}

			coap_packet_t & message()
			{
				return message_;
			}

			void set_message(const coap_packet_t &message)
			{
				message_ = message;
			}

			node_id_t correspondent() const
			{
				return correspondent_;
			}

			void set_correspondent( node_id_t correspondent)
			{
				correspondent_ = correspondent;
			}

			uint8_t retransmit_count() const
			{
				return retransmit_count_;
			}

			void set_retransmit_count(uint8_t retransmit_count)
			{
				retransmit_count_ = retransmit_count;
			}

			uint16_t retransmit_timeout() const
			{
				return retransmit_timeout_;
			}

			void set_retransmit_timeout( uint16_t retransmit_timeout )
			{
				retransmit_timeout_ = retransmit_timeout;
			}

			uint16_t increase_retransmit_count()
			{
				++retransmit_count_;
				retransmit_timeout_ *= 2;
				return retransmit_timeout_;
			}

			bool ack_received() const
			{
				return ack_received_;
			}

			void set_ack_received(bool ack_received)
			{
				ack_received_ = ack_received;
			}

			ReceivedMessage * response_received() const
			{
				return response_;
			}

			void set_response_received(ReceivedMessage & response)
			{
				response_ = &response;
			}

			coapreceiver_delegate_t sender_callback() const
			{
				return sender_callback_;
			}

			void set_sender_callback( const coapreceiver_delegate_t &callback )
			{
				sender_callback_ = callback;
			}

		private:
			coap_packet_t message_;
			// in this case the receiver
			node_id_t correspondent_;
			uint8_t retransmit_count_;
			uint16_t retransmit_timeout_;
			bool ack_received_;
			ReceivedMessage * response_;
			coapreceiver_delegate_t sender_callback_;
		};

		class CoapResource
		{
		public:
			bool operator==( const CoapResource &other ) const
			{
				return ( this->resource_path() == other.resource_path() && this->callback() == other.callback() );
			}

			bool operator!=( const CoapResource &other ) const
			{
				return !( *this == other );
			}

			CoapResource()
			{
				resource_path_ = string_t();
				callback_ = coapreceiver_delegate_t();
			}

			CoapResource( string_t path, coapreceiver_delegate_t callback)
			{
				set_resource_path( path );
				set_callback( callback );
			}

			void set_resource_path( string_t path)
			{
				resource_path_ = path;
			}

			string_t resource_path() const
			{
				return resource_path_;
			}

			void set_callback( coapreceiver_delegate_t callback )
			{
				callback_ = callback;
			}

			coapreceiver_delegate_t callback() const
			{
				return callback_;
			}

		private:
			string_t resource_path_;
			coapreceiver_delegate_t callback_;
		};

		class TimerAction
		{
		public:
			TimerType type_;
			void * message_;

			TimerAction() { type_ = TIMER_NONE; message_ = NULL; }
			~TimerAction() {}
			bool operator==( const TimerAction &rhs ) { return ( this->type_ == rhs.type_ && this->message_ == rhs.message_ ); }
			bool operator!=( const TimerAction &rhs ) { return !( *this == rhs ); }
		};

		typedef list_static<OsModel, ReceivedMessage, COAPRADIO_RECEIVED_LIST_SIZE> received_list_t;
		typedef list_static<OsModel, SentMessage, COAPRADIO_SENT_LIST_SIZE> sent_list_t;

		Radio *radio_;
		Timer *timer_;
		Debug *debug_;
		Rand *rand_;
		int recv_callback_id_; // callback for receive function
		sent_list_t sent_;
		received_list_t received_;
		vector_static<OsModel, CoapResource, COAPRADIO_RESOURCES_SIZE> resources_;
		vector_static<OsModel, TimerAction, COAPRADIO_TIMER_ACTION_SIZE> timers_;

		coap_msg_id_t msg_id_;
		coap_token_t token_;

		CoapRadio( const self_type &rhs );

		coap_msg_id_t msg_id();
		coap_token_t token();

		template <typename T, list_size_t N>
		T * queue_message(T message, list_static<OsModel_P, T, N> &queue);

		template <typename T, list_size_t N>
		T* find_message_by_id (node_id_t correspondent, coap_msg_id_t id, list_static<OsModel_P, T, N> &queue);
		template <typename T, list_size_t N>
		T* find_message_by_token (node_id_t correspondent, const OpaqueData& token, const list_static<OsModel_P, T, N> &queue);

		void handle_response( ReceivedMessage& message, SentMessage *request = NULL );

		void handle_request( ReceivedMessage& message );

		void ack(ReceivedMessage& message );

		void timeout ( void * action );

		unsigned int reg_timer_action( TimerAction *action );

		void unreg_timer_action( unsigned int idx );

		void error_response( int error, ReceivedMessage& message );

	};


// public

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::CoapRadio()
	{
		//init();
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename Timer_P,
		typename Debug_P,
		typename Rand_P,
		typename String_T>
	CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::~CoapRadio()
	{

	}

	template<typename OsModel_P,
		typename Radio_P,
		typename Timer_P,
		typename Debug_P,
		typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::destruct()
	{
		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::init()
	{
		// TODO: WTF!!!
		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::init(Radio& radio,
				Timer& timer,
				Debug& debug,
				Rand& rand )
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::init\n");
#endif
		radio_ = &radio;
		timer_ = &timer;
		debug_ = &debug;
		rand_ = &rand;

		rand_->srand( radio_->id() );

		// random initial message ID and token
		msg_id_ = (*rand_)();
		token_ = (*rand_)();
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::init> initial msg_id %i, initial token %i\n", msg_id_, token_);
#endif
		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::enable_radio()
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::enable_radio\n");
#endif
#ifdef DEBUG_COAPRADIO_PC
			cout << "CoapRadio::enable_radio\n";
#endif
		//enable normal radio
		radio_->enable_radio();
		// register receive callback to normal radio
		recv_callback_id_ = radio_->template reg_recv_callback<self_t,
			&self_t::receive > ( this );

#ifdef DEBUG_COAPRADIO_PC
			cout << "CoapRadio::enable_radio> Callback id " << recv_callback_id_ <<"\n";
#endif

		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::disable_radio()
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::disable\n");
#endif
		radio_->unreg_recv_callback(recv_callback_id_);
		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	typename Radio_P::node_id_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::id ()
	{
		return radio_->id();
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send (node_id_t receiver, size_t len, block_data_t *data )
	{
#ifdef DEBUG_COAPRADIO_TEST
			debug_->debug( "Node %x -- CoapRadio::sending %x message of length %i \n", id(), receiver, len );
#endif
#if COAP_PREFACE_MSG_ID == 1
		block_data_t buf[len+1];
		buf[0] = CoapMsgId;
		memcpy( buf + 1, data, len );
		radio_->send(receiver, len + 1, buf);
#else
		radio_->send(receiver, len, data);
#endif

		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_as_is(node_id_t receiver, const coap_packet_t & message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::send_coap_as_is> receiver %i, type %i, code %i.%02i, msg_id %i\n",
					receiver, message.type(), ( ( message.code() & 0xE0 ) >> 5 ), ( message.code() & 0x1F ), message.msg_id() );
#endif
#ifdef DEBUG_COAPRADIO_PC
		cout << "CoapRadio::send_coap_as_is> msg_type " << (int) message.type()
				<< ", code " << (int) ( ( message.code() & 0xE0 ) >> 5 ) << "." << (int) ( message.code() & 0x1F )
				<< ", msg id " << message.msg_id()
				<< "\n";
#endif
		block_data_t buf[message.serialize_length()];

		size_t len = message.serialize(buf);
		int status = send(receiver, len, buf);

		if(status != SUCCESS )
			return NULL;

		SentMessage & sent = *( queue_message(SentMessage(), sent_) );
		sent.set_correspondent( receiver );
		sent.set_message( message );
		sent.set_sender_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
		uint16_t response_timeout = (uint16_t) ((*rand_)( (COAP_MAX_RESPONSE_TIMEOUT - COAP_RESPONSE_TIMEOUT) ) + COAP_RESPONSE_TIMEOUT);
		sent.set_retransmit_timeout( response_timeout );

		if( message.type() == COAP_MSG_TYPE_CON )
		{
			TimerAction action;
			action.type_ = TIMER_RETRANSMIT;
			action.message_ = (void*) &sent;
			void* actionp = (void*) reg_timer_action( &action );

			timer_->template set_timer<self_type, &self_type::timeout>( sent.retransmit_timeout(), this, actionp );
		}

		return &(sent.message());
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_gen_msg_id(node_id_t receiver, coap_packet_t & message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::gen_msg_id> \n");
#endif
#ifdef DEBUG_COAPRADIO_PC
		cout << "CoapRadio::gen_msg_id> \n";
#endif
		message.set_msg_id( this->msg_id() );
		return send_coap_as_is<T, TMethod>( receiver, message, callback );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_gen_msg_id_token(node_id_t receiver, coap_packet_t & message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::gen_msg_id_token> \n");
#endif
#ifdef DEBUG_COAPRADIO_PC
		cout << "CoapRadio::gen_msg_id_token> \n";
#endif
		OpaqueData token;
		coap_token_t raw_token = this->token();
		token.set( ( uint8_t* ) &raw_token, sizeof( coap_token_t ) );
		message.set_token( token );
		return send_coap_gen_msg_id<T, TMethod>( receiver, message, callback );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::receive(node_id_t from, size_t len, block_data_t * data)
	{
#ifdef DEBUG_COAPRADIO_PC
		cout << "CoapRadio::receive> Node " << (int) radio_->id() << ": "
				<< "From "
				<< ((((uint32_t) from.addr()) & 0xff000000) >> 24)
				<< "."
				<< ((((uint32_t) from.addr()) & 0x00ff0000) >> 16)
				<< "."
				<< ((((uint32_t) from.addr()) & 0x0000ff00) >> 8)
				<< "."
				<< (((uint32_t) from.addr()) & 0x000000ff)
				<< ":" << from.port()
				<< ", Len " << len
				<< "\n";
#endif
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %x -- CoapRadio::receive> received message from %i, length %i\n", id(), from, len );
#endif
		// do not receive own messages
		if (radio_->id() == from) {
#ifdef DEBUG_COAPRADIO
			debug_->debug( "CoapRadio::receive> Node %i: own message received\n", radio_->id());
#endif
#ifdef DEBUG_COAPRADIO_PC
			cout << "CoapRadio::receive> Node " << (int) radio_->id() << ": own message received\n";
#endif
			return;
		}
		if (len > 0 )
		{
			size_t msg_id_t_size = 0;
#if COAP_PREFACE_MSG_ID == 1
			message_id_t msg_id = read<OsModel, block_data_t, message_id_t>( data );
			msg_id_t_size = sizeof( message_id_t );
			if( msg_id == CoapMsgId )
			{
#endif
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> received coap message from %i\n", radio_->id(), from);
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::receive> received coap message from "
						<< ((((uint32_t) from.addr()) & 0xff000000) >> 24)
						<< "."
						<< ((((uint32_t) from.addr()) & 0x00ff0000) >> 16)
						<< "."
						<< ((((uint32_t) from.addr()) & 0x0000ff00) >> 8)
						<< "."
						<< (((uint32_t) from.addr()) & 0x000000ff)
						<< "\n";
				cout << "raw: " << hex;
				for( size_t m = 0; m < len; ++m )
				{
					cout << (int) *(data + m) << " ";
				}
				cout << dec << "\n";
#endif

				coap_packet_t packet;
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> parsing message, length %i\n", id(), (len - msg_id_t_size) );
#endif
#ifdef DEBUG_COAPRADIO_TEST_XX
				debug_->debug( "Node %i -- CoapRadio::receive> pointer %x passing %x to parse\n",
						id(), (uint32_t) data, (uint32_t) (data + msg_id_t_size ) );
#endif
				int err_code = packet.parse_message( data + msg_id_t_size, len - msg_id_t_size );
#ifdef DEBUG_COAPRADIO
				debug_->debug( "CoapRadio::receive> parse_message returned %i\n", err_code );
#endif
#ifdef DEBUG_COAPRADIO_TEST_XX
				debug_->debug( "Node %x -- CoapRadio::receive> packet: pointer %x\n", id(), (uint32_t) packet.datastream_p );
				debug_->debug( "Node %x -- CoapRadio::receive> packet: version %x type %x code %x msg_id %x\n",
						id(), packet.version_, packet.type_, packet.code_ , packet.msg_id_ );
#endif
				if( err_code == SUCCESS )
				{
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> successfully parsed message: \n", id());
				debug_->debug( "Node %i -- CoapRadio::receive> type %i, code %i.%02i, msg_id %i \n",
							id(), packet.type(), ( ( packet.code() & 0xE0 ) >> 5 ), ( packet.code() & 0x1F ), packet.msg_id() );
				debug_->debug( "Node %i -- CoapRadio::receive> token: ", id());
				OpaqueData debug_token;
				packet.token(debug_token);
				for(size_t m = 0; m < debug_token.length(); ++m)
				{
					debug_->debug("%x ", debug_token.value()[m] );
				}
				debug_->debug( "\n" );
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::receive> Successfully parsed message:\n";
				cout << "CoapRadio::receive> type "
						<< packet.type()
						<< ", code "
						<< ( ( packet.code() & 0xE0 ) >> 5 ) << "." << ( packet.code() & 0x1F )
						<< ", msg id " << packet.msg_id()
						<< "\n";
#endif
					ReceivedMessage *deduplication;
					// Only act if this message hasn't been received yet
					if( (deduplication = find_message_by_id(from, packet.msg_id(), received_)) == NULL )
					{
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> message is not duplicate!\n", radio_->id());
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::receive> message is not duplicate!\n";
#endif
						ReceivedMessage& received_message = *( queue_message( ReceivedMessage( packet, from ), received_ ) );

						SentMessage *request;

						if ( packet.type() == COAP_MSG_TYPE_RST )
						{
							request = find_message_by_id( from, packet.msg_id(), sent_ );
							if( request != NULL )
								(*request).sender_callback()( received_message );
							return;
						}
						else if( packet.type() == COAP_MSG_TYPE_ACK )
						{
							request = find_message_by_id( from, packet.msg_id(), sent_ );

							if ( request != NULL )
							{
								(*request).set_ack_received( true );
								// piggy-backed response, give it to whoever sent the request
								if( packet.is_response() )
									handle_response( received_message, request );
							}
						}
						else
						{
#ifdef DEBUG_COAPRADIO_TEST_XX
				debug_->debug( "Node %i -- CoapRadio::receive> code %i\n", radio_->id(), packet.code() );
#endif
							if( packet.is_request() )
							{
								handle_request( received_message );
							}
							else if ( packet.is_response() )
							{
								handle_response( received_message );
							}
							else if( packet.type() == COAP_MSG_TYPE_CON )
							{
								char * error_description = NULL;
								int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
								char error_description_str[COAP_ERROR_STRING_LEN];
								len = sprintf( error_description, "Unknown Code %i", packet.code() );
								error_description = error_description_str;
#endif
								reply( received_message, (block_data_t*) error_description, len, COAP_CODE_NOT_IMPLEMENTED );
							}
						}
					}
					else
					{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::receive> duplicate found\n";
#endif
						// if it's confirmable we might want to hurry sending an ACK
						if( packet.type() == COAP_MSG_TYPE_CON )
							ack( *deduplication );
						// if the response was piggybacked it was already resent by the line above
						if( deduplication->response_sent() != NULL
								&& deduplication->response_sent()->type() != COAP_MSG_TYPE_ACK)
						{
							block_data_t buf[ deduplication->response_sent()->serialize_length() ];

							deduplication->response_sent()->serialize(buf);
							send(deduplication->correspondent(), deduplication->response_sent()->serialize_length(), buf);
						}
					}
				}
				else if( err_code == coap_packet_t::ERR_NOT_COAP
				         || err_code == coap_packet_t::ERR_WRONG_COAP_VERSION )
				{
					// ignore
					// wrong Coap Version is a good indicator for "isn't
					// actually CoAP", so better ignore
				}
				else
				{
					ReceivedMessage& received_error = *( queue_message( ReceivedMessage( packet, from ), received_ ) );
					error_response( err_code, received_error );
				}
#if COAP_PREFACE_MSG_ID == 1
			}
#ifdef DEBUG_COAPRADIO
			else
			{
				debug_->debug( "CoapRadio::receive> Node %i: received message that was not coap!\n", radio_->id());
			}
#endif
#endif
		}
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::rst( node_id_t receiver, coap_msg_id_t id )
	{
		coap_packet_t rstp;
		rstp.set_type( COAP_MSG_TYPE_RST );
		rstp.set_msg_id( id );
		return send_coap_as_is<self_type, &self_type::receive_coap>( receiver, rstp, this );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::receive_coap(ReceivedMessage& message)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::receive_coap> received message from %i\n", message.correspondent() );
#endif
		//TODO
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::reg_resource_callback( string_t resource_path, T *callback )
	{

		if ( resources_.empty() )
			resources_.assign( COAPRADIO_RESOURCES_SIZE, CoapResource() );

		for ( unsigned int i = 0; i < resources_.size(); ++i )
		{
			if ( resources_.at(i) == CoapResource() )
			{
				resources_.at(i).set_resource_path( resource_path );
				resources_.at(i).set_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::reg_resource_callback> registered callback at %i\n", id(), i );
#endif
				return i;
			}
		}

		return -1;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::unreg_resource_callback( int idx )
	{
		resources_.at(idx) = CoapResource();
		return SUCCESS;
	}


	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::get(node_id_t receiver,
			string_t uri_path,
			string_t uri_query,
			T *callback,
			bool confirmable,
			string_t uri_host,
			uint16_t uri_port)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::get> \n");
#endif
		return request<T, TMethod>( receiver, COAP_CODE_GET ,uri_path, uri_query, callback, NULL, 0, confirmable, uri_host, uri_port );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::put(node_id_t receiver,
			string_t uri_path,
			string_t uri_query,
			T *callback,
			uint8_t* payload,
			size_t payload_length,
			bool confirmable,
			string_t uri_host,
			uint16_t uri_port)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::put> \n");
#endif
		return request<T, TMethod>( receiver, COAP_CODE_PUT ,uri_path, uri_query, callback, payload, payload_length, confirmable, uri_host, uri_port );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::post(node_id_t receiver,
			string_t uri_path,
			string_t uri_query,
			T *callback,
			uint8_t* payload,
			size_t payload_length,
			bool confirmable,
			string_t uri_host,
			uint16_t uri_port)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::post> \n");
#endif
		return request<T, TMethod>( receiver, COAP_CODE_POST ,uri_path, uri_query, callback, payload, payload_length, confirmable, uri_host, uri_port );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::del(node_id_t receiver,
			string_t uri_path,
			string_t uri_query,
			T *callback,
			bool confirmable,
			string_t uri_host,
			uint16_t uri_port)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::delete> \n");
#endif
		return request<T, TMethod>( receiver, COAP_CODE_DELETE ,uri_path, uri_query, callback, NULL, 0, confirmable, uri_host, uri_port );
	}

	template<typename OsModel_P,
				typename Radio_P,
				typename Timer_P,
				typename Debug_P,
				typename Rand_P,
				typename String_T>
	template <class T, void (T::*TMethod)( typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ReceivedMessage& ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::request(node_id_t receiver,
				CoapCode code,
				string_t uri_path,
				string_t uri_query,
				T *callback,
				uint8_t* payload,
				size_t payload_length,
				bool confirmable,
				string_t uri_host,
				uint16_t uri_port)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::request> \n");
#endif
		coap_packet_t pack;

		pack.set_code( code );
		if( !pack.is_request() )
		{
			// TODO ordentlichen Fehler schmeißen?
			return NULL;
		}

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::request> setting path\n");
#endif

		pack.set_uri_path( uri_path );
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::request> setting query\n");
#endif
		pack.set_uri_query( uri_query );

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::request> setting data\n");
#endif

		pack.set_data( payload, payload_length );

		confirmable ? pack.set_type( COAP_MSG_TYPE_CON ) : pack.set_type( COAP_MSG_TYPE_NON );

		if( uri_host != string_t() )
		{
			pack.set_option( COAP_OPT_URI_HOST, uri_host );
		}

		pack.set_uri_port( uri_port );

		return send_coap_gen_msg_id_token<T, TMethod>(receiver, pack, callback );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::reply(ReceivedMessage &req_msg,
				uint8_t* payload,
				size_t payload_length,
				CoapCode code )
	{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::reply> \n";
#endif
		coap_packet_t *sendstatus = NULL;
		coap_packet_t & request = req_msg.message();
		coap_packet_t reply;
		OpaqueData token;
		request.token( token );

		reply.set_token( token );

		if( request.type() == COAP_MSG_TYPE_CON || request.type() == COAP_MSG_TYPE_NON )
			reply.set_type( request.type() );
		else
			return NULL;
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> type is CON or NON\n");
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::reply> type is CON or NON\n";
#endif
		reply.set_code( code );
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> setting payload\n");
#endif
		reply.set_data( payload, payload_length );

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> payload is %i bytes long: ");
		for(size_t m = 0; m < payload_length; ++m )
		{
			debug_->debug("%i ", payload[m] );
		}
		debug_->debug("\n");
#endif
		if( request.type() == COAP_MSG_TYPE_CON && req_msg.ack_sent() == NULL )
		{
			// no ACK has been sent yet, piggybacked response is possible
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> sending piggybacked response\n");
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::reply> sending piggybacked response\n";
#endif
			reply.set_type( COAP_MSG_TYPE_ACK );
			reply.set_msg_id( request.msg_id() );
			sendstatus = send_coap_as_is<self_type, &self_type::receive_coap>( req_msg.correspondent(), reply, this );
			if( sendstatus != NULL )
			{
				req_msg.set_ack_sent( sendstatus );
				req_msg.set_response_sent( sendstatus );
			}
		}
		else
		{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> sending seperate response\n");
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::reply> sending seperate response\n";
#endif
			sendstatus = send_coap_gen_msg_id<self_type, &self_type::receive_coap>( req_msg.correspondent(), reply, this );
			if( sendstatus != NULL )
			{
				req_msg.set_response_sent( sendstatus );
			}
		}

		return sendstatus;
	}


// private
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	coap_msg_id_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::msg_id()
	{
		return(msg_id_++);
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	coap_token_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::token()
	{
		return(token_++);
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <typename T, list_size_t N>
	T * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::queue_message(T message, list_static<OsModel_P, T, N> &queue)
	{
		if( queue.full() )
		{
			queue.pop_back();
		}
		queue.push_front( message );
		return &(queue.front());
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <typename T, list_size_t N>
	T* CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::find_message_by_id
			(node_id_t correspondent, coap_msg_id_t id, list_static<OsModel_P, T, N> &queue)
	{
		typename list_static<OsModel_P, T, N>::iterator it = queue.begin();
		for(; it != queue.end(); ++it)
		{
			if( (*it).message().msg_id() == id && (*it).correspondent() == correspondent )
				return &(*it);
		}

		return NULL;

	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <typename T, list_size_t N>
	T* CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::find_message_by_token
		(node_id_t correspondent, const OpaqueData &token, const list_static<OsModel_P, T, N> &queue)
	{
		OpaqueData current_token;
		typename list_static<OsModel_P, T, N>::iterator it = queue.begin();
		for(; it != queue.end(); ++it)
		{
			if( (*it).correspondent() == correspondent )
			{
				(*it).message().token( current_token );
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::find_by_token> Token 1: ", id() );
		for(size_t m = 0; m < token.length(); ++m)
		{
			debug_->debug("%x ", token.value()[m] );
		}
		debug_->debug("\nNode %i -- CoapRadio::find_by_token> Token 2: ", id() );
		for(size_t m = 0; m < current_token.length(); ++m)
		{
			debug_->debug("%x ", current_token.value()[m] );
		}
		debug_->debug("\n");
#endif
				if( current_token == token )
				{
					return &(*it);
				}
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::find_by_token> not equal\n", id() );
#endif
			}
		}
		return NULL;
	}

	// the request-pointer can be a candidate for a matching request, determined by a previous search by message id.
	// If it doesn't turn out to be matching, find_message_by_token has to be called
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::handle_response( ReceivedMessage& message, SentMessage *request )
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_response> correspondent %i, msg id %i\n",
				id(),
				message.correspondent(),
				message.message().msg_id() );
#endif
		OpaqueData request_token, response_token;
		message.message().token( response_token );

#ifdef DEBUG_COAPRADIO
		if( response_token == OpaqueData() )
			debug_->debug("Node %i -- CoapRadio::handle_response> got empty token O_o\n" );
#endif

		// see if the given request candidate is the matching request, otherwise find the matching request by token
		if( request != NULL )
		{
			(*request).message().token(request_token);
		}

		if( request == NULL || request_token != response_token )
		{
			request = find_message_by_token( message.correspondent(), response_token, sent_ );
			if( request == NULL )
			{
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_response> couldn't find matching request\n", id() );
#endif
				// can't match response
				if( message.message().type() == COAP_MSG_TYPE_CON )
				{
					message.set_response_sent( rst( message.correspondent(), message.message().msg_id() ) );
				}
				return;
			}
			(*request).message().token(request_token);
		}

		if( message.message().type() == COAP_MSG_TYPE_CON )
		{
			ack( message );
		}

		if( request->sender_callback() && request->sender_callback().obj_ptr() != NULL )
		{
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_response> calling response-handler\n", id() );
#endif
			(*request).set_response_received( message );
			(*request).sender_callback()( message );
		}

	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::handle_request( ReceivedMessage& message )
	{
		// don't send an ACK right away, instead wait a little to give the
		// resource a chance to send a piggybacked response
		if( message.message().type() == COAP_MSG_TYPE_CON )
		{
			TimerAction action;
			action.type_ = TIMER_ACK;
			action.message_ = (void*) &message;
			void* actionp = (void*) reg_timer_action( &action );

			timer_->template set_timer<self_type, &self_type::timeout>( COAP_ACK_GRACE_PERIOD, this, actionp );
		}

		string_t available_res;
		// TODO: we're looking at the first path segment only, subresources should be handled by their parents
		string_t request_res = message.message().uri_path();
		bool resource_found = false;
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_request> looking for resource '%s'\n", id(), request_res.c_str() );
#endif
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::handle_request> looking for resource '"
						<< request_res.c_str() << "'\n";
#endif
		for(size_t i = 0; i < resources_.size(); ++i )
		{
			if( resources_.at(i) != CoapResource() && resources_.at(i).callback() && resources_.at(i).callback().obj_ptr() != NULL )
			{
				available_res = resources_.at(i).resource_path();
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_request> found resource '%s'\n", id(), available_res.c_str() );
#endif
				// in order to match a resource, the requested uri must match a resource, or it must be a sub-element of a resource,
				// which means the next symbol in the request must be a slash
				if( ( available_res.length() == request_res.length()
						&& strncmp(available_res.c_str(), request_res.c_str(), available_res.length()) == 0 )
					|| (available_res.length() < request_res.length()
						&& strncmp(available_res.c_str(), request_res.c_str(), available_res.length()) == 0
						&& strncmp(request_res.c_str() + available_res.length(), "/", 1)) )
				{
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_request> resource match, calling callback\n", id() );
#endif
					resources_.at(i).callback()( message );
					resource_found = true;
				}
			}
		}
		if( !resource_found )
		{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapRadio::handle_request> resource not found, sending 4.04'\n";
#endif
			char * error_description = NULL;
			int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
			char error_description_str[COAP_ERROR_STRING_LEN];
			len = sprintf(error_description, "Resource %s not found.", request_res.c_str() );
			error_description = error_description_str;
#endif
			reply( message, (uint8_t*) error_description, len, COAP_CODE_NOT_FOUND );
		}
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::ack( ReceivedMessage& message )
	{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::ack> msg_id " << message.message().msg_id() << "\n";
#endif
		if( message.ack_sent() != NULL )
		{
			block_data_t buf[ message.ack_sent()->serialize_length() ];

			message.ack_sent()->serialize(buf);
			send(message.correspondent(), message.ack_sent()->serialize_length(), buf);
			return;
		}
		coap_packet_t ackp;
		ackp.set_type( COAP_MSG_TYPE_ACK );
		ackp.set_msg_id( message.message().msg_id() );
		coap_packet_t * sent = send_coap_as_is<self_type, &self_type::receive_coap>(message.correspondent(), ackp, this );
		message.set_ack_sent( sent );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::timeout ( void * action )
	{
		TimerAction& act = timers_.at( (unsigned int) action );

		if ( act.type_ == TIMER_RETRANSMIT )
		{
			SentMessage *sent = ((SentMessage*) act.message_);
			if( !sent->ack_received() && sent->response_received() == NULL)
			{
				size_t length = sent->message().serialize_length();
				block_data_t buf[length];
				sent->message().serialize(buf);
				int status = send(sent->correspondent(), length, buf);

				if(status != SUCCESS )
				{
					timer_->template set_timer<self_type, &self_type::timeout>( 1000, this, action );
					return;
				}

				uint16_t retransmit_timeout = sent->increase_retransmit_count();
				if(sent->retransmit_count() < COAP_MAX_RETRANSMIT )
				{
					timer_->template set_timer<self_type, &self_type::timeout>( retransmit_timeout, this, action );
				}
			}
			else
			{
				unreg_timer_action( ( unsigned int ) action );
			}
		}
		else if ( act.type_ == TIMER_ACK )
		{
			ReceivedMessage *sent = ((ReceivedMessage*) act.message_);
			if( sent->ack_sent() == NULL )
			{
				ack( (*sent) );
			}
			else
			{
				unreg_timer_action( ( unsigned int ) action );
			}
		}
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename Timer_P,
		typename Debug_P,
		typename Rand_P,
		typename String_T>
	unsigned int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::reg_timer_action( TimerAction *action )
	{
		if ( timers_.empty() )
		{
			timers_.assign( COAPRADIO_TIMER_ACTION_SIZE, TimerAction() );
		}

		for ( unsigned int i = 0; i < timers_.size(); ++i )
		{
			if ( timers_.at(i) == TimerAction() )
			{
				timers_.at(i) = (*action);
				return i;
			}
		}

		return NULL;
	}

	template<typename OsModel_P,
	typename Radio_P,
	typename Timer_P,
	typename Debug_P,
	typename Rand_P,
	typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::unreg_timer_action( unsigned int idx )
	{
		timers_.at( idx ) = TimerAction();
	}

	template<typename OsModel_P,
	typename Radio_P,
	typename Timer_P,
	typename Debug_P,
	typename Rand_P,
	typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::error_response( int error, ReceivedMessage& message )
	{
		coap_packet_t packet = message.message();
		CoapCode err_coap_code;
		CoapOptionNum err_optnum;
		packet.get_error_context( err_coap_code, err_optnum);
		block_data_t * error_description = NULL;
		int len = 0;
#if ( COAP_HUMAN_READABLE_ERRORS == 0 )
		block_data_t error_description_uint[ sizeof( error ) + sizeof( CoapOptionNum ) ];
		len = write<OsModel , block_data_t , typeof(error) >( error_description_uint, error );
		len += write<OsModel , block_data_t , CoapOptionNum >( error_description_uint + len, err_optnum );
		error_description = error_description_uint;
#elif ( COAP_HUMAN_READABLE_ERRORS == 1 )
		char error_description_str[COAP_ERROR_STRING_LEN];
		switch( error)
		{
		case ERR_OPTIONS_EXCEED_PACKET_LENGTH:
			len = sprintf(error_description_str, "Error: Options exceed packet length, last parsed option: %i", err_optnum );
			break;
		case ERR_UNKNOWN_CRITICAL_OPTION:
			len = sprintf(error_description_str, "Error: Unknown critical option %i ", err_optnum );
			break;
		case ERR_MULTIPLE_OCCURENCES_OF_CRITICAL_OPTION:
			len = sprintf(error_description_str, "Error: Undue multiple occurences of option %i ", err_optnum );
			break;
		case ERR_EMPTY_STRING_OPTION:
			len = sprintf(error_description_str, "Error: Empty String option %i ", err_optnum );
			break;
		case ERR_NOT_COAP:
			// should not happen as these are already sorted out in receive()
			break;
		case ERR_WRONG_COAP_VERSION:
			// should not happen as these are already sorted out in receive()
			break;
		default:
			len = sprintf(error_description_str, "Error: Unknown error %i, last option before error: %i ", error, err_optnum );
		}
		error_description = (block_data_t *) error_description_str;
#endif
		reply( message, error_description, len, err_coap_code );
	}
}


#endif // COAPRADIO_H
