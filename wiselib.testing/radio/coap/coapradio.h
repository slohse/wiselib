#ifndef COAPRADIO_H
#define COAPRADIO_H

#include "coap.h"
#include "coap_packet.h"
#include "util/delegates/delegate.hpp"

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

		void receive_coap(node_id_t from, coap_packet_t message);

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

			const coapreceiver_delegate_t& sender_callback() const
			{
				return sender_callback_;
			}

			void set_sender_callback( coapreceiver_delegate_t &callback )
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
				ack_ = false;
				response_ = false;
			}

			const coap_packet_t& message() const
			{
				return message;
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

		struct coapreceiver
		{
			// TODO:
			// Typ des Receivers (bekommt der nur die Daten, oder das ganze Paket?)
			// Callback Pointer oder sowas
			// string der die Resource bezeichnet
		};

		Radio *radio_;
		Timer *timer_;
		Debug *debug_;
		Rand *rand_;
		int recv_callback_id_; // callback for receive function
		list_static<OsModel, SentMessage, COAPRADIO_SENT_LIST_SIZE> sent_;
		list_static<OsModel, ReceivedMessage, COAPRADIO_RECEIVED_LIST_SIZE> received_;

		coap_msg_id_t msg_id_;
		coap_token_t token_;

		coap_msg_id_t msg_id();
		coap_token_t token();

		template <typename T, list_size_t N>
		void queue_message(T message, list_static<OsModel_P, T, N> &queue);

		template <typename T, list_size_t N>
		T* find_message_by_id (node_id_t correspondent, coap_msg_id_t id, list_static<OsModel_P, T, N> &queue);
		template <typename T, list_size_t N>
		T* find_message_by_token (node_id_t correspondent, OpaqueData id, list_static<OsModel_P, T, N> &queue);

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
		coapreceiver_delegate_t delegate = coapreceiver_delegate_t::template from_method<T, TMethod>( callback );
		sent.sender_callback( delegate );
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
					switch( packet.type() )
					{
					case COAP_MSG_TYPE_ACK:
						SentMessage *request = find_message_by_id( from, packet.msg_id(), sent_ );

						if ( request != NULL )
						{
							(*request).set_ack_received( true );
							// piggy-backed response
							if( packet.is_response() )
							{

							}
						}
						break;


// TODO: nachdenken ob man hier einigen Code gemeinsam nutzen kann
/*						if( !packet.is_response() )
							break;
						// otherwise it's a piggy-backed response and can be handled like a
*/
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

/*
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P,
			typename Rand_P>
	coap_token_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P, Rand_P>::get(node_id_t receiver, )
	{

	}
*/

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

}


#endif // COAPRADIO_H
