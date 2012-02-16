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
	: public RadioBase<OsModel_P,
		typename Radio_P::node_id_t,
		typename Radio_P::size_t,
		typename Radio_P::block_data_t> {

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

		typedef typename CoapPacket<OsModel, Radio, string_t>::coap_packet_t coap_packet_t;
		typedef typename CoapPacket<OsModel, Radio, string_t>::coap_packet_r coap_packet_r;
		typedef typename CoapPacket<OsModel, Radio, string_t>::coap_packet_p coap_packet_p;

		typedef delegate2<void, node_id_t, const coap_packet_r> coapreceiver_delegate_t;

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
		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		coap_packet_t* send_coap_as_is(node_id_t receiver, const coap_packet_r message, T *callback);

		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		coap_packet_t* send_coap_gen_msg_id(node_id_t receiver, coap_packet_r message, T *callback);

		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		coap_packet_t* send_coap_gen_msg_id_token(node_id_t receiver, coap_packet_r message, T *callback);
		
		coap_packet_t* rst( node_id_t receiver, coap_msg_id_t id );

		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		int reg_resource_callback( string_t resource_path, T *callback );

		int unreg_recv_callback( int idx );

		void receive_coap(node_id_t from, const coap_packet_r message);

		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		coap_packet_t* get(node_id_t receiver,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					uint8_t* payload = NULL,
					size_t payload_length = 0,
					bool confirmable = false,
					string_t uri_host = string_t(),
					uint16_t uri_port = COAP_STD_PORT);

		template<class T, void (T::*TMethod)(node_id_t, const coap_packet_r)>
		coap_packet_t* request(node_id_t receiver,
					CoapCode code,
					string_t uri_path,
					string_t uri_query,
					T *callback,
					uint8_t* payload,
					size_t payload_length,
					bool confirmable,
					string_t uri_host,
					uint16_t uri_port);

		coap_packet_t* reply(coap_packet_t request,
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
		class SentMessage
		{
		public:
			SentMessage()
			{
				retransmit_count_ = 0;
				ack_received_ = false;
				sender_callback_ = coapreceiver_delegate_t();
			}

			coap_packet_r message() const
			{
				return message_;
			}

			coap_packet_r message()
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
			coapreceiver_delegate_t sender_callback_;
		};

		class ReceivedMessage
		{
		public:
			ReceivedMessage()
			{
				message_ = coap_packet_t();
				ack_ = NULL;
				response_ = false;
			}

			ReceivedMessage( const coap_packet_t &packet, node_id_t from )
			{
				message_ = packet;
				correspondent_ = from;
				ack_ = NULL;
				response_ = false;
			}

			coap_packet_r message() const
			{
				return message_;
			}

			coap_packet_r message()
			{
				return message_;
			}

			void set_message( const coap_packet_t &message)
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

			coap_packet_t * ack_sent() const
			{
				return ack_;
			}

			void set_ack_sent( coap_packet_t *ack )
			{
				ack_ = ack;
			}

			bool response_sent() const
			{
				return response_;
			}

			void set_response_sent(bool response)
			{
				response_ = response;
			}

		private:
			coap_packet_t message_;
			// in this case the sender
			node_id_t correspondent_;
			coap_packet_t *ack_;
			bool response_;
			// TODO: empfangszeit? (Freshness)
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

		coap_packet_t* reply( ReceivedMessage& req_msg,
				uint8_t* payload,
				size_t payload_length,
				CoapCode code = COAP_CODE_CONTENT );

		coap_msg_id_t msg_id();
		coap_token_t token();

		template <typename T, list_size_t N>
		T * queue_message(T message, list_static<OsModel_P, T, N> &queue);

		template <typename T, list_size_t N>
		T* find_message_by_id (node_id_t correspondent, coap_msg_id_t id, list_static<OsModel_P, T, N> &queue);
		template <typename T, list_size_t N>
		T* find_message_by_token (node_id_t correspondent, OpaqueData token, list_static<OsModel_P, T, N> &queue);

		void handle_response( node_id_t from, ReceivedMessage& message, SentMessage *request = NULL );

		void handle_request( node_id_t from, ReceivedMessage& message );

		void ack(ReceivedMessage& message );

		void timeout ( void * action );

		TimerAction * reg_timer_action( TimerAction *action );

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
		debug_->debug("CoapRadio::enable\n");
#endif
		//enable normal radio
		radio_->enable_radio();
		// register receive callback to normal radio
		recv_callback_id_ = radio_->template reg_recv_callback<self_t,
			&self_t::receive > ( this );

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
#ifdef DEBUG_COAPRADIO
			debug_->debug( "Node %i -- CoapRadio::send \n", id() );
#endif
		block_data_t buf[len+1];
		buf[0] = CoapMsgId;
		memcpy( buf + 1, data, len );
#ifdef DEBUG_COAPRADIO
			debug_->debug( "Node %i -- CoapRadio::send> sending: 0x ", id() );
			for(size_t i = 0; i < len; ++i)
			{
				debug_->debug( "%x ", data[i] );
			}
			debug_->debug( "\n" );
#endif
		radio_->send(receiver, len + 1, buf);

		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_as_is(node_id_t receiver, const coap_packet_r message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::send_coap_as_is> receiver %i, type %i, code %i.%02i, msg_id %i\n",
					receiver, message.type(), ( ( message.code() & 0xE0 ) >> 5 ), ( message.code() & 0x1F ), message.msg_id() );
#endif
		block_data_t buf[message.serialize_length()];

		message.serialize(buf);
		int status = send(receiver, message.serialize_length(), buf);

		if(status != SUCCESS )
			return NULL;

		SentMessage sent;
		sent.set_correspondent( receiver );
		sent.set_message( message );
		sent.set_sender_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
		uint16_t response_timeout = (uint16_t) ((*rand_)( (COAP_MAX_RESPONSE_TIMEOUT - COAP_RESPONSE_TIMEOUT) ) + COAP_RESPONSE_TIMEOUT);
		sent.set_retransmit_timeout( response_timeout );
		SentMessage * sentp = queue_message(sent, sent_);

		if( message.type() == COAP_MSG_TYPE_CON )
		{
			TimerAction action;
			action.type_ = TIMER_RETRANSMIT;
			action.message_ = (void*) sentp;
			TimerAction * actionp = reg_timer_action( &action );

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
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_gen_msg_id(node_id_t receiver, coap_packet_r message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::gen_msg_id> \n");
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
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::send_coap_gen_msg_id_token(node_id_t receiver, coap_packet_r message, T *callback)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::gen_msg_id_token> \n");
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
		// do not receive own messages
		if (radio_->id() == from) {
#ifdef DEBUG_COAPRADIO
			debug_->debug( "CoapRadio::receive> Node %i: own message received\n", radio_->id());
#endif
			return;
		}
		if (len > 0 )
		{
			message_id_t msg_id = read<OsModel, block_data_t, message_id_t>( data );
			if( msg_id == CoapMsgId )
			{
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> received coap message from %i\n", radio_->id(), from);
#endif

				// notify those who want raw data
				notify_receivers( from, len - sizeof( message_id_t ), data + sizeof( message_id_t ) );

				coap_packet_t packet;
				int err_code = packet.parse_message( data + sizeof( message_id_t ), len - sizeof( message_id_t ) );
				if( err_code == SUCCESS )
				{
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> successfully parsed message: \n", radio_->id());
				debug_->debug( "Node %i -- CoapRadio::receive> type %i, code %i.%02i, msg_id %i \n",
							radio_->id(), packet.type(), ( ( packet.code() & 0xE0 ) >> 5 ), ( packet.code() & 0x1F ), packet.msg_id() );
#endif
					ReceivedMessage *deduplication;
					// Only act if this message hasn't been received yet, otherwise ignore
					if( (deduplication = find_message_by_id(from, packet.msg_id(), received_)) == NULL )
					{
#ifdef DEBUG_COAPRADIO
				debug_->debug( "Node %i -- CoapRadio::receive> message is not duplicate!\n", radio_->id());
#endif
						ReceivedMessage received_message( packet, from );
						queue_message( received_message, received_ );
						SentMessage *request;

						if ( packet.type() == COAP_MSG_TYPE_RST )
						{
							request = find_message_by_id( from, packet.msg_id(), sent_ );
							if( request != NULL )
								(*request).sender_callback()( from, packet );
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
									handle_response( from, received_message, request );
							}
						}
						else
						{
							if( packet.is_request() )
							{
								handle_request( from, received_message );
							}
							else if ( packet.is_response() )
							{
								handle_response( from, received_message );
							}
							else if( packet.type() == COAP_MSG_TYPE_CON )
							{
								char buf[ MAX_STRING_LENGTH ];
								int buf_len = sprintf( buf, "Unknown Code %i", packet.code() );
								reply( received_message, (block_data_t*) buf, buf_len, COAP_CODE_NOT_IMPLEMENTED );
							}
						}
					}
					else
					{
						// if it's confirmable we might want to hurry sending an ACK
						if( packet.type() == COAP_MSG_TYPE_CON )
							ack( *deduplication );
					}
				}
				else if ( err_code == coap_packet_t::ERR_CON_RESPONSE || err_code == coap_packet_t::ERR_RST )
				{
					send_coap_as_is<self_type, &self_type::receive_coap>(from, packet, this );
				}
				// every other case, including coap_packet_t::ERR_IGNORE_MSG
				else
				{
					// ignore
					return;
				}
			}
#ifdef DEBUG_COAPRADIO
			else
			{
				debug_->debug( "CoapRadio::receive> Node %i: received message that was not coap!\n", radio_->id());
			}
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
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::receive_coap(node_id_t from, const coap_packet_r message)
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::receive_coap> received message from %i\n", from );
#endif
		//TODO
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
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
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::unreg_recv_callback( int idx )
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
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::get(node_id_t receiver,
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
		debug_->debug("CoapRadio::get> \n");
#endif
		return request<T, TMethod>( receiver, COAP_CODE_GET ,uri_path, uri_query, callback, payload, payload_length, confirmable, uri_host, uri_port );
	}

	template<typename OsModel_P,
				typename Radio_P,
				typename Timer_P,
				typename Debug_P,
				typename Rand_P,
				typename String_T>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, const typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_r ) >
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

		// if no host is supplied the physical address of the node is used
		if( uri_host == string_t() )
		{
			char buf[MAX_STRING_LENGTH];
			sprintf( buf, "%i", id() );
			uri_host = string_t( buf );
		}
		pack.set_option( COAP_OPT_URI_HOST, uri_host );

		pack.set_option( COAP_OPT_URI_PORT, uri_port );


		return send_coap_gen_msg_id_token<T, TMethod>(receiver, pack, callback );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	typename CoapPacket<OsModel_P, Radio_P, String_T>::coap_packet_t * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::reply(coap_packet_t request,
				uint8_t* payload,
				size_t payload_length,
				CoapCode code )
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> \n");
#endif
		ReceivedMessage *req_mes = NULL;
		typename received_list_t::iterator it = received_.begin();
		for(; it != received_.end(); ++it)
		{
			if( request == ( (coap_packet_t) (*it).message() ) )
			{
				req_mes = &(*it);
				break;
			}
		}


		// TODO: ordentlichen Fehler schmeissen?
		if( req_mes == NULL )
			return NULL;

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> found matching request\n");
#endif
		return reply( (*req_mes), payload, payload_length, code );
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
		coap_packet_t *sendstatus = NULL;
		coap_packet_r request = req_msg.message();
		coap_packet_t reply;
		OpaqueData token;
		if ( request.token( token ) != SUCCESS )
			return NULL;

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> found token\n");
#endif
		reply.set_token( token );
		if( request.type() == COAP_MSG_TYPE_CON || request.type() == COAP_MSG_TYPE_NON )
			reply.set_type( request.type() );
		else
			return NULL;
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> type is CON or NON\n");
#endif
		reply.set_code( code );
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> setting payload\n");
#endif
		reply.set_data( payload, payload_length );

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> setting payload\n");
#endif
		if( request.type() == COAP_MSG_TYPE_CON && req_msg.ack_sent() == NULL )
		{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> sending piggybacked response\n");
#endif
			reply.set_type( COAP_MSG_TYPE_ACK );
			reply.set_msg_id( request.msg_id() );
			sendstatus = send_coap_as_is<self_type, &self_type::receive_coap>( req_msg.correspondent(), reply, this );
			if( sendstatus != NULL )
			{
				req_msg.set_ack_sent( sendstatus );
				req_msg.set_response_sent( true );
			}
		}
		else
		{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapRadio::reply> sending seperate response\n");
#endif
			sendstatus = send_coap_gen_msg_id<self_type, &self_type::receive_coap>( req_msg.correspondent(), reply, this );
			if( sendstatus != NULL )
			{
				req_msg.set_response_sent( true );
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
		(node_id_t correspondent, OpaqueData token, list_static<OsModel_P, T, N> &queue)
	{
		OpaqueData current_token;
		typename list_static<OsModel_P, T, N>::iterator it = queue.begin();
		for(; it != queue.end(); ++it)
		{
			if( (*it).correspondent() == correspondent && (*it).message().get_option( COAP_OPT_TOKEN, current_token ) == SUCCESS )
			{
				if( current_token == token )
				{
					return &(*it);
				}
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
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::handle_response( node_id_t from, ReceivedMessage& message, SentMessage *request )
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_response> \n", id() );
#endif
		OpaqueData request_token, response_token;

		// No token --> can't match response
		if( (message.message()).token( response_token ) != SUCCESS )
		{
			if( message.message().type() == COAP_MSG_TYPE_CON )
			{
				rst( from, message.message().msg_id() );
				message.set_response_sent(true);
			}
			return;
		}

		// see if the given request candidate is the matching request, otherwise find the matching request by token
		if( !( request != NULL  && (*request).message().token(request_token) == SUCCESS  && request_token == response_token ) )
		{
			request = find_message_by_token( from, response_token, sent_ );
		}

		if( ( request != NULL  && (*request).message().token(request_token) == SUCCESS
				&& request_token == response_token ) && request->sender_callback() && request->sender_callback().obj_ptr() != NULL )
		{
			(*request).sender_callback()( from, message.message() );
		}
		else
		{
			// can't match response
			if( message.message().type() == COAP_MSG_TYPE_CON )
			{
				rst( from, message.message().msg_id() );
				message.set_response_sent(true);
			}
			return;
		}

	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P,
			typename String_T>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::handle_request( node_id_t from, ReceivedMessage& message )
	{
		string_t available_res;
		string_t request_res = message.message().uri_path();
		bool resource_found = false;
#ifdef DEBUG_COAPRADIO
		debug_->debug("Node %i -- CoapRadio::handle_request> looking for resource '%s'\n", id(), request_res.c_str() );
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
					resources_.at(i).callback()( from, message.message() );
					resource_found = true;
				}
			}
		}
		if( !resource_found )
		{
			char error_description[MAX_STRING_LENGTH];
			int len = sprintf(error_description, "Resource %s not found.", request_res.c_str() );
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
		TimerAction *act = (TimerAction *) action;

		if ( act->type_ == TIMER_RETRANSMIT )
		{
			SentMessage *sent = ((SentMessage*) act->message_);
			if( !sent->ack_received() )
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
				(*act) = TimerAction();
			}
		}
		else if ( act->type_ == TIMER_ACK )
		{
			ReceivedMessage *sent = ((ReceivedMessage*) act->message_);
			if( sent->ack_sent() == NULL )
			{
				ack( (*sent) );
			}
			else
			{
				(*act) = TimerAction();
			}
		}
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename Timer_P,
		typename Debug_P,
		typename Rand_P,
		typename String_T>
	typename CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::TimerAction * CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P, String_T>::reg_timer_action( TimerAction *action )
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
				return &(timers_.at(i));
			}
		}

		return NULL;
	}
}


#endif // COAPRADIO_H
