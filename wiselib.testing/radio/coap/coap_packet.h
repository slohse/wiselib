#ifndef COAP_PACKET_H
#define COAP_PACKET_H

#include "coap.h"


namespace wiselib
{
	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	class CoapPacket
	{
	public:
		typedef OsModel_P OsModel;
		typedef typename OsModel_P::Debug Debug;
		typedef Radio_P Radio;
		typedef typename Radio::block_data_t block_data_t;
		typedef String_T string_t;

		typedef CoapPacket<OsModel_P, Radio_P, String_T> self_type;
		typedef self_type* self_pointer_t;
		typedef self_type coap_packet_t;

		///@name Construction / Destruction
		///@{
		CoapPacket( );
		CoapPacket( const coap_packet_t &rhs );
		~CoapPacket();
		///@}

		coap_packet_t& operator=(const coap_packet_t &rhs);

		void init();

		/**
		 * Returns the CoAP version number of the packet
		 * @return CoAP version number
		 */
		uint8_t version() const;

		/**
		 * Sets the CoAP version number of the packet. DO NOT set it to anything other than COAP_VERSION unless you know what you are doing!
		 * Note: the corresponding field in the CoAP packet is 2 bit wide. Setting it to anything greater than 3 is pointless.
		 * @param version version number
		 */
		void set_version( uint8_t version );

		/**
		 * Returns the type of the packet. Can be Confirmable, Non-Confirmable, Acknowledgement or Reset.
		 * @return message type
		 */
		CoapType type() const;

		/**
		 * Sets the type of the packet. Can be COAP_MSG_TYPE_CON, COAP_MSG_TYPE_NON, COAP_MSG_TYPE_ACK or COAP_MSG_TYPE_RST.
		 * @param type message type
		 */
		void set_type( CoapType type );

		/**
		 * Returns the code of the packet. Can be a request (1-31), response (64-191) or empty (0). (all other values are reserved)
		 * For details refer to the CoAP Code Registry section of the CoAP draft.
		 * @return code of the message
		 */
		CoapCode code() const;

		/**
		 * Returns whether the packet is a request
		 * @return true if packet is a request, false otherwise
		 */
		bool is_request() const;

		/**
		 * Returns whether the packet is a response
		 * @return true if packet is a response, false otherwise
		 */
		bool is_response() const;

		/**
		 * Sets the code of the packet. Can be a request (1-31), response (64-191) or empty (0). (all other values are reserved)
		 * For details refer to the CoAP Code Registry section of the CoAP draft.
		 * @param code code of the packet
		 */
		void set_code( CoapCode code );

		/**
		 * Returns the message ID by which message duplication can be detected and request/response matching can be done.
		 * @return the message ID
		 */
		coap_msg_id_t msg_id() const;

		/**
		 * Sets the message ID by which message duplication can be detected and request/response matching can be done.
		 * @param msg_id new message ID
		 */
		void set_msg_id( coap_msg_id_t msg_id );

		/**
		 * Returns a pointer to the payload section of the packet
		 * @return pointer to payload
		 */
		block_data_t* data();
		const block_data_t* data() const;

		/**
		 * Returns the length of the payload
		 * @return payload length
		 */
		size_t data_length() const;

		/**
		 * Sets the payload
		 * @param data the payload
		 * @param length payload length
		 */
		void set_data( block_data_t* data , size_t length);


	private:
		// points to beginning of payload
		block_data_t *payload_;
		block_data_t *end_of_options_;
		size_t data_length_;
		coap_msg_id_t msg_id_;
		CoapType type_;
		CoapCode code_;
		uint8_t version_;

		bool opt_if_none_match_;
		block_data_t* options_[COAP_OPTION_ARRAY_SIZE];
		// contains Options and Data
		block_data_t storage_[COAP_STORAGE_SIZE];

		void set_option(CoapOptionNum num, block_data_t *serial_opt, size_t len);
		uint8_t next_fencepost_delta(uint8_t previous_opt_number) const;

	};


	// Implementation starts here
	template<typename OsModel_P,
	typename Radio_P,
	typename String_T>
	CoapPacket<OsModel_P, Radio_P, String_T>& CoapPacket<OsModel_P, Radio_P, String_T>::operator=(const CoapPacket<OsModel_P, Radio_P, String_T> &rhs)
	{
		// TODO
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapPacket<OsModel_P, Radio_P, String_T>::CoapPacket()
	{
		init();
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapPacket<OsModel_P, Radio_P, String_T>::CoapPacket( const coap_packet_t &rhs)
	{
		init();
		*this = rhs;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapPacket<OsModel_P, Radio_P, String_T>::~CoapPacket()
	{
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::init()
	{
		version_ = COAP_VERSION;
		// TODO: sinnvollen Default festlegen und dann ein COAP_MSG_DEFAULT_TYPE Makro anlegen oder so
		type_ = COAP_MSG_TYPE_NON;
		code_ = COAP_CODE_EMPTY;
		msg_id_ = 0;
		opt_if_none_match_ = false;
		for(size_t i = 0; i < COAP_OPTION_ARRAY_SIZE; ++i)
		{
			options_[i] = NULL;
		}

		payload_ = NULL;
		end_of_options_ = storage_;
		data_length_ = 0;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint8_t CoapPacket<OsModel_P, Radio_P, String_T>::version() const
	{
		return version_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_version( uint8_t version )
	{
		version_ = version & 0x03;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapType CoapPacket<OsModel_P, Radio_P, String_T>::type() const
	{
		return type_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_type( CoapType type )
	{
		type_ = type;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapCode CoapPacket<OsModel_P, Radio_P, String_T>::code() const
	{
		return code_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_code( CoapCode code )
	{
		code_ = code;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	bool CoapPacket<OsModel_P, Radio_P, String_T>::is_request() const
	{
		return( code() >= COAP_REQUEST_CODE_RANGE_MIN && code() <= COAP_REQUEST_CODE_RANGE_MAX );
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	bool CoapPacket<OsModel_P, Radio_P, String_T>::is_response() const
	{
		return( code() >= COAP_RESPONSE_CODE_RANGE_MIN && code() <= COAP_RESPONSE_CODE_RANGE_MAX );
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint16_t CoapPacket<OsModel_P, Radio_P, String_T>::msg_id() const
	{
		return msg_id_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_msg_id( uint16_t msg_id )
	{
		msg_id_ = msg_id;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_data( block_data_t* data , size_t length)
	{
		// we put data at the very end
		data_length_ = length;
		payload_ = storage_ + ((COAP_STORAGE_SIZE - 1) - data_length_ );
		memcpy(payload_, data, data_length_ );

	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	typename Radio_P::block_data_t * CoapPacket<OsModel_P, Radio_P, String_T>::data()
	{
		return payload_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	const typename Radio_P::block_data_t * CoapPacket<OsModel_P, Radio_P, String_T>::data() const
	{
		return payload_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::data_length() const
	{
		return data_length_;
	}



//-----------------------------------------------------------------------------
// Private methods start here

	template<typename OsModel_P,
			typename Radio_P,
			typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_option(CoapOptionNum num, block_data_t *serial_opt, size_t len)
	{
		CoapOptionNum prev = 0;
		uint8_t fencepost = 0;
		// data + header
		size_t bytes_needed = len + 1;
		if( len > 14 )
			++bytes_needed;

		block_data_t put_here = end_of_options_;
		// there are options set
		if( put_here > storage_ )
		{
			// look for the next bigger option - this is where we need to start
			// moving things further back
			for( size_t i = (size_t) num; i < COAP_OPTION_ARRAY_SIZE; ++i )
			{
				if( options_[i] != NULL )
				{
					put_here = options_[i];
					break;
				}
			}

			for( size_t i = (size_t) num; i > 0; --i )
			{
				if( options_[i] != NULL )
				{
					prev = (CoapOptionNum) i;
					break;
				}
			}

			if( num - prev > 15 )
			{
				++bytes_needed;
				fencepost = next_fencepost_delta( prev );
			}
		}

		// move following options back
		// TODO: check if payload is violated...
		if( put_here < end_of_options_ )
		{
			memmove(put_here + bytes_needed,
			        put_here,
			        (size_t) (end_of_options_ - put_here));
		}

		memcpy( put_here + (bytes_needed - len), serial_opt, len );
		end_of_options_ += bytes_needed;
		if( fencepost != 0)
		{
			*put_here = fencepost << 4;
			prev = fencepost;
			options_[fencepost] = put_here;
			++put_here;
		}
		if( len < 15 )
		{
			*put_here = ((num - prev) << 4) | (len & 0x0f);
		}
		else
		{
			*put_here = ((num - prev) << 4) | 0x0f;
			*(put_here + 1) = (len - 15);
		}

		while( put_here < end_of_options_ )
		{
			options_[ prev + (((*put_here) & 0xf0) >> 4) ] = put_here;
			len = (*put_here) & 0x0f;
			if( len < 15 )
				put_here += len + 1;
			else
				put_here +=  *(put_here + 1) + 16;
		}
	}

	template<typename OsModel_P,
	typename Radio_P,
	typename String_T>
	uint8_t CoapPacket<OsModel_P, Radio_P, String_T>::next_fencepost_delta(uint8_t previous_opt_number) const
	{
		return ( COAP_OPT_FENCEPOST - ( (previous_opt_number) % COAP_OPT_FENCEPOST ) );
	}

}




#endif // COAP_PACKET_H
