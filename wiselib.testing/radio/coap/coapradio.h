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
	typename Rand_P = typename OsModel_P::Rand>
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

		typedef typename Radio::node_id_t node_id_t;
		typedef typename Radio::size_t size_t;
		typedef typename Radio::block_data_t block_data_t;
		typedef typename Radio::message_id_t message_id_t;
		
		typedef CoapRadio<OsModel, Radio, Timer, Debug, Rand> self_type;
		typedef self_type* self_pointer_t;

		typedef CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P> self_t;

		typedef typename CoapPacket<OsModel, Radio>::coap_packet_t coap_packet_t;

		typedef delegate2<void, node_id_t, coap_packet_t> coapreceiver_delegate_t;

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
		template<class T, void (T::*TMethod)(node_id_t, coap_packet_t)>
		int send_coap_as_is(node_id_t receiver, coap_packet_t &message, T *callback);
		
		int rst( node_id_t receiver, coap_msg_id_t id );

		template<class T, void (T::*TMethod)(node_id_t, coap_packet_t)>
		int reg_resource_callback( StaticString resource_path, T *callback );

		int unreg_recv_callback( int idx );

		void receive_coap(node_id_t from, coap_packet_t message);

		template<class T, void (T::*TMethod)(node_id_t, coap_packet_t)>
		void get(node_id_t receiver,
					StaticString uri_path,
					StaticString uri_query,
					T *callback,
					bool confirmable = false,
					uint16 uri_port = COAP_STD_PORT);

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
			}

			coap_packet_t message() const
			{
				return message_;
			}

			void set_message(coap_packet_t &message)
			{
				message_ = message;
			}

			node_id_t correspondent() const
			{
				return correspondent();
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

			uint8_t increase_retransmit_count()
			{
				return ++retransmit_count_;
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

			void set_sender_callback( coapreceiver_delegate_t callback )
			{
				sender_callback_ = callback;
			}

		private:
			coap_packet_t message_;
			// in this case the receiver
			node_id_t correspondent_;
			uint8_t retransmit_count_;
			bool ack_received_;
			coapreceiver_delegate_t sender_callback_;
		};

		class ReceivedMessage
		{
		public:
			ReceivedMessage()
			{
				message_ = coap_packet_t();
				ack_ = false;
				response_ = false;
			}

			ReceivedMessage( coap_packet_t packet )
			{
				message_ = packet;
				ack_ = false;
				response_ = false;
			}

			coap_packet_t message() const
			{
				return message_;
			}

			void set_message(coap_packet_t message)
			{
				message_ = message;
			}

			node_id_t correspondent() const
			{
				return correspondent();
			}

			void set_correspondent( node_id_t correspondent)
			{
				correspondent_ = correspondent;
			}

			bool ack_sent() const
			{
				return ack_;
			}

			void set_ack_sent(bool ack)
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
			bool ack_;
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
				resource_path_ = StaticString();
				callback_ = coapreceiver_delegate_t();
			}

			CoapResource( StaticString path, coapreceiver_delegate_t callback)
			{
				set_resource_path( path );
				set_callback( callback );
			}

			void set_resource_path( StaticString path)
			{
				resource_path_ = path;
			}

			StaticString resource_path() const
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
			StaticString resource_path_;
			coapreceiver_delegate_t callback_;
		};

		Radio *radio_;
		Timer *timer_;
		Debug *debug_;
		Rand *rand_;
		int recv_callback_id_; // callback for receive function
		list_static<OsModel, SentMessage, COAPRADIO_SENT_LIST_SIZE> sent_;
		list_static<OsModel, ReceivedMessage, COAPRADIO_RECEIVED_LIST_SIZE> received_;
		vector_static<OsModel, CoapResource, COAPRADIO_RESOURCES_SIZE> resources_;

		coap_msg_id_t msg_id_;
		coap_token_t token_;

		coap_msg_id_t msg_id();
		coap_token_t token();

		template <typename T, list_size_t N>
		void queue_message(T message, list_static<OsModel_P, T, N> &queue);

		template <typename T, list_size_t N>
		T* find_message_by_id (node_id_t correspondent, coap_msg_id_t id, list_static<OsModel_P, T, N> &queue);
		template <typename T, list_size_t N>
		T* find_message_by_token (node_id_t correspondent, OpaqueData token, list_static<OsModel_P, T, N> &queue);

		void handle_response( node_id_t from, ReceivedMessage& message, SentMessage *request = NULL );

		void handle_request( node_id_t from, ReceivedMessage& message );

	};


// public
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::init()
	{
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
		typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::destruct()
	{
		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::init(Radio& radio,
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
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::enable_radio()
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
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::disable_radio()
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
			typename Rand_P>
	typename Radio_P::node_id_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::id ()
	{
		return radio_->id();
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::send (node_id_t receiver, size_t len, block_data_t *data )
	{
#ifdef DEBUG_COAPRADIO
			debug_->debug( "CoapRadio::send");
#endif
		block_data_t buf[len+1];
		buf[0] = CoapMsgId;
		memcpy( buf + 1, data, len );
		radio_->send(receiver, len + 1, buf);

		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, typename CoapPacket<OsModel_P, Radio_P>::coap_packet_t ) >
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::send_coap_as_is(node_id_t receiver, coap_packet_t &message, T *callback)
	{
		block_data_t buf[message.serialize_length()];

		message.serialize(buf);
		int status = send(receiver, message.serialize_length(), buf);

		if(status != SUCCESS )
			return status;

		SentMessage sent;
		sent.set_correspondent( receiver );
		sent.set_message( message );
		sent.set_sender_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
		queue_message(sent, sent_);

		// TODO: Timer starten für CON-Retransmits

		return SUCCESS;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::receive(node_id_t from, size_t len, block_data_t * data)
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
				debug_->debug( "CoapRadio::receive> Node %i: received coap message!\n", radio_->id());
#endif

				// notify those who want raw data
				notify_receivers( from, len - sizeof( message_id_t ), data + sizeof( message_id_t ) );

				CoapPacket<OsModel, Radio> packet;
				int err_code = packet.parse_message( data + sizeof( message_id_t ), len - sizeof( message_id_t ) );
				if( err_code == SUCCESS )
				{
					ReceivedMessage received_message(packet);
					queue_message( received_message, received_ );
					SentMessage *request;
					switch( packet.type() )
					{
					case COAP_MSG_TYPE_ACK:
						request = find_message_by_id( from, packet.msg_id(), sent_ );

						if ( request != NULL )
						{
							(*request).set_ack_received( true );
							// piggy-backed response, give it to whoever sent the request
							if( packet.is_response() )
								handle_response( from, received_message, request );
						}
						break;
// TODO: nachdenken ob man hier einigen Code gemeinsam nutzen kann
					case COAP_MSG_TYPE_CON:
						if( packet.is_request() )
						{
							handle_request( from, received_message );
						}
						else if ( packet.is_response() )
						{
							handle_response( from, received_message );
						}
						else
						{
							// TODO: unknown code, send 5.01 Not Implemented
						}
						break;
					case COAP_MSG_TYPE_NON:
						if( packet.is_request() )
						{
							handle_request( from, received_message );
						}
						else if ( packet.is_response() )
						{
							handle_response( from, received_message );
						}
						// drop unknown codes silently
						break;
					case COAP_MSG_TYPE_RST:
						request = find_message_by_id( from, packet.msg_id(), sent_ );
						if( request != NULL )
							(*request).sender_callback()( from, packet );
						break;
					default:
						break;
					}
				}
				else if ( err_code == CoapPacket<OsModel, Radio>::ERR_CON_RESPONSE || err_code == CoapPacket<OsModel, Radio>::ERR_RST )
				{
					// TODO
				}
				// every other case, including CoapPacket<OsModel, Radio>::ERR_IGNORE_MSG
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
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::rst( node_id_t receiver, coap_msg_id_t id )
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
			typename Rand_P>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::receive_coap(node_id_t from, coap_packet_t message)
	{
		//TODO
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, typename CoapPacket<OsModel_P, Radio_P>::coap_packet_t ) >
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::reg_resource_callback( StaticString resource_path, T *callback )
	{

		if ( resources_.empty() )
			resources_.assign( COAPRADIO_RESOURCES_SIZE, CoapResource() );

		for ( unsigned int i = 0; i < resources_.size(); ++i )
		{
			if ( resources_.at(i) == CoapResource() )
			{
				resources_.at(i).set_resource_path( resource_path );
				resources_.at(i).set_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
				return i;
			}
		}

		return -1;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	int CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::unreg_recv_callback( int idx )
	{
		resources_.at(idx) = CoapResource();
		return SUCCESS;
	}


	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	template <class T, void (T::*TMethod)( typename Radio_P::node_id_t, typename CoapPacket<OsModel_P, Radio_P>::coap_packet_t ) >
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::get(node_id_t receiver,
			StaticString uri_path,
			StaticString uri_query,
			T *callback,
			bool confirmable,
			uint16 uri_port)
	{

	}


// private
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	coap_msg_id_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::msg_id()
	{
		return(msg_id_++);
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	coap_token_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::token()
	{
		return(token_++);
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	template <typename T, list_size_t N>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::queue_message(T message, list_static<OsModel_P, T, N> &queue)
	{
		if( queue.full() )
		{
			queue.pop_back();
		}
		queue.push_front( message );
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	template <typename T, list_size_t N>
	T* CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::find_message_by_id
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
			typename Rand_P>
	template <typename T, list_size_t N>
	T* CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::find_message_by_token
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
			typename Rand_P>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::handle_response( node_id_t from, ReceivedMessage& message, SentMessage *request )
	{
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

		if( ( request != NULL  && (*request).message().token(request_token) == SUCCESS  && request_token == response_token ) )
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
			typename Rand_P>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::handle_request( node_id_t from, ReceivedMessage& message )
	{
		StaticString available_res;
		StaticString request_res = message.message().uri_path();
		for(size_t i = 0; i < resources_.size(); ++i )
		{
			if( resources_.at(i) != CoapResource() )
			{
				available_res = resources_.at(i).resource_path();
				// in order to match a resource, the requested uri must match a resource, or it must be a sub-element of a resource,
				// which means the next symbol in the request must be a slash
				if( ( available_res.length() == request_res.length()
						&& strncmp(available_res.c_str(), request_res.c_str(), available_res.length()) == 0 )
					|| (available_res.length() < request_res.length()
						&& strncmp(available_res.c_str(), request_res.c_str(), available_res.length()) == 0
						&& strncmp(request_res.c_str() + available_res.length(), "/", 1)) )
				{
					resources_.at(i).callback()( from, message.message() );
				}
			}
		}
	}
}


#endif // COAPRADIO_H
