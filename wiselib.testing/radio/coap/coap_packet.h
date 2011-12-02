#ifndef COAP_PACKET_H
#define COAP_PACKET_H

#include "coap.h"

namespace wiselib
{
	template<typename OsModel_P>
	class CoapPacket
	{

		// TODO: maybe when I have spare time...
//		template<typename T>
//		class CoapOption
//		{
//		public:
//			CoapOption() { T default_value; set(0, default_value); }
//			CoapOption( uint8_t option_number, T value) { set(option_number, value); }
//			virtual ~CoapOption() {}
//			uint8_t option_number() {return option_number_;}
//			T value() {return value_; }
//			void set( uint8_t option_number, T value ) { option_number_ = option_number; value_ = value; }
//
//		private:
//			uint8_t option_number_;
//			T value_;
//		};

		class UintOption
		{
		public:
			UintOption() { set(0,0); }
			UintOption( uint8_t option_number, uint32_t value) { set(option_number, value); }
			virtual ~UintOption() {}
			uint8_t option_number() {return option_number_;}
			uint32_t value() {return value_; }
			void set( uint8_t option_number, uint32_t value ) { option_number_ = option_number; value_ = value; }

		private:
			uint8_t option_number_;
			uint32_t value_;

		};

		class StringOption
		{
		public:
			StringOption() { set(0, StaticString("")); }
			StringOption( uint8_t option_number, StaticString value) { set( option_number, value); }
			virtual ~StringOption() {}
			uint8_t option_number() {return option_number_;}
			StaticString value() {return value_; }
			void set( uint8_t option_number, StaticString value ) { option_number_ = option_number; value_ = value; }

		private:
			uint8_t option_number_;
			StaticString value_;
		};

		class OpaqueOption
		{
		public:
			OpaqueOption() { set(0, NULL, 0); }
			OpaqueOption( uint8_t option_number, uint8_t* value, int length ) { set(option_number, value, length); }
			virtual ~OpaqueOption() {}
			uint8_t option_number() const {return option_number_;}
			size_t length() const { return value_.length(); }
			uint8_t *value() { return value_.value(); }
			uint8_t const *value() const { return value_.value(); }
			void set( uint8_t option_number, uint8_t* value, size_t length) { option_number_ = option_number; value_.set(value, length); }

		private:
			uint8_t option_number_;
			OpaqueData value_;
		};

	public:
		typedef OsModel_P OsModel;
		typedef typename OsModel_P::Debug Debug;
		
		///@name Construction / Destruction
		///@{
		CoapPacket();
		CoapPacket( uint8_t *datastream, size_t length );
		~CoapPacket();
		///@}

		void init();

		/**
		 * Returns the CoAP version number of the packet
		 * @return CoAP version number
		 */
		uint8_t version();

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
		uint8_t type();

		/**
		 * Sets the type of the packet. Can be COAP_MSG_TYPE_CON, COAP_MSG_TYPE_NON, COAP_MSG_TYPE_ACK or COAP_MSG_TYPE_RST.
		 * @param type message type
		 */
		void set_type( uint8_t type );

		/**
		 * Returns the number of options that are set in this packet.
		 * @return number of options set
		 */
		uint8_t option_count();

		/**
		 * Returns the code of the packet. Can be a request (1-31), response (64-191) or empty (0). (all other values are reserved)
		 * For details refer to the CoAP Code Registry section of the CoAP draft.
		 * @return code of the message
		 */
		uint8_t code();

		/**
		 * Sets the code of the packet. Can be a request (1-31), response (64-191) or empty (0). (all other values are reserved)
		 * For details refer to the CoAP Code Registry section of the CoAP draft.
		 * @param code code of the packet
		 */
		void set_code( uint8_t code );

		/**
		 * Returns the message ID by which message duplication can be detected and request/response matching can be done.
		 * @return the message ID
		 */
		uint16_t msg_id();

		/**
		 * Sets the message ID by which message duplication can be detected and request/response matching can be done.
		 * @param msg_id new message ID
		 */
		void set_msg_id( uint16_t msg_id );

		/**
		 * Returns a pointer to the payload section of the packet
		 * @return pointer to payload
		 */
		uint8_t* data();

		/**
		 * Returns the length of the payload
		 * @return payload length
		 */
		size_t data_length();

		/**
		 * Sets the payload
		 * @param data the payload
		 * @param length payload length
		 */
		void set_data( uint8_t* data , size_t length);

		/**
		 * Calculates the length of the message if it were serialized in the current sate
		 * @return the expected length of the serialized message
		 */
		size_t serialize_length();

		/**
		 * Serializes the packet so it can be sent over the radio.
		 * @param pointer to where the serialized packet will be written.
		 * @return length of the packet
		 */
		size_t serialize( uint8_t *datastream );

		uint32_t what_options_are_set();

		int set_option( uint8_t option_number, uint32_t value );
		int set_option( uint8_t option_number, StaticString value );
		int set_option( uint8_t option_number, uint8_t *value, size_t length );

		int add_option( uint8_t option_number, uint32_t value );
		int add_option( uint8_t option_number, StaticString value );
		int add_option( uint8_t option_number, uint8_t *value, size_t length );

		int get_option( uint8_t option_number, uint32_t &value );
		int get_option( uint8_t option_number, StaticString &value );
		int get_option( uint8_t option_number, OpaqueData &value );

		template <list_size_t N>
		int get_options( uint8_t option_number, list_static<OsModel_P, uint32_t, N> &values );

		template <list_size_t N>
		int get_options( uint8_t option_number, list_static<OsModel_P, StaticString, N> &values );

		template <list_size_t N>
		int get_options( uint8_t option_number, list_static<OsModel_P, OpaqueData, N> &values );

		int remove_option( uint8_t option_number );

		bool opt_if_none_match();
		void set_opt_if_none_match( bool opt_if_none_match );

		enum error_codes
		{
			// inherited from concepts::BasicReturnValues_concept
			SUCCESS = OsModel::SUCCESS,
			ERR_UNSPEC = OsModel::ERR_UNSPEC,
			ERR_NOTIMPL = OsModel::ERR_NOTIMPL,
			// coap specific
			ERR_WRONG_TYPE,
			ERR_UNKNOWN_OPT,
			ERR_OPT_NOT_SET
		};

	private:
		typename Debug::self_pointer_t debug_;

		uint8_t version_;
		uint8_t type_;
		uint8_t code_;
		uint16_t msg_id_;

		// options
		list_static<OsModel, UintOption, COAP_LIST_SIZE_UINT> uint_options_;
		list_static<OsModel, StringOption, COAP_LIST_SIZE_STRING> string_options_;
		list_static<OsModel, OpaqueOption, COAP_LIST_SIZE_OPAQUE> opaque_options_;
		bool opt_if_none_match_;

		uint8_t* data_;
		size_t data_length_;

		// methods:
		void parse_message( uint8_t *datastream, size_t length );
		void parse_option( uint8_t option_number, uint16_t option_length, uint8_t* value);
		inline uint8_t next_fencepost(uint8_t previous_opt_number);
		inline void fenceposting( uint8_t option_number, uint8_t &previous_opt_number, uint8_t *datastream, size_t &offset );
		inline void optlength( size_t length, uint8_t *datastream, size_t &offset );

		// methods dealing with Options
		template <typename T, list_size_t N>
		void set_option( list_static<OsModel_P, T, N> &options, T option );

		template <typename T, list_size_t N>
		void add_option( list_static<OsModel_P, T, N> &options, T option );

		template <typename T, list_size_t N>
		int get_option( uint8_t option_number, T &value, list_static<OsModel_P, T, N> &options );

		template <typename T, list_size_t N, list_size_t M>
		int get_options( uint8_t option_number, list_static<OsModel_P, T, N> &values, list_static<OsModel_P, T, M> &options );

		template <typename T, list_size_t N>
		void remove_option( list_static<OsModel_P, T, N> &options, uint8_t option_number );

		size_t serialize_option( uint8_t *datastream, uint8_t previous_option_number, OpaqueOption &opt );
		size_t serialize_option( uint8_t *datastream, uint8_t previous_option_number, StringOption &opt );
		size_t serialize_option( uint8_t *datastream, uint8_t previous_option_number, UintOption &opt );

		uint32_t deserialize_uint( uint8_t *datastream, size_t length);

	};


// Implementation starts here

	template<typename OsModel_P>
	CoapPacket<OsModel_P>::CoapPacket()
	{
		init();
	}

	template<typename OsModel_P>
	CoapPacket<OsModel_P>::CoapPacket( uint8_t *datastream, size_t length )
	{
		init();
		parse_message( datastream, length );
	}

	template<typename OsModel_P>
	CoapPacket<OsModel_P>::~CoapPacket()
	{
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::init()
	{
//		debug_ = OsModel::;

		version_ = COAP_VERSION;
		// TODO: sinnvollen Default festlegen und dann ein COAP_MSG_DEFAULT_TYPE Makro anlegen oder so
		type_ = COAP_MSG_TYPE_NON;
		code_ = COAP_CODE_EMPTY;
		msg_id_ = 0;
		opt_if_none_match_ = false;
		uint_options_.clear();
		string_options_.clear();
		opaque_options_.clear();

		data_length_ = 0;
	}

	template<typename OsModel_P>
	uint8_t CoapPacket<OsModel_P>::version()
	{
		return version_;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_version( uint8_t version )
	{
		version_ = version & 0x03;
	}

	template<typename OsModel_P>
	uint8_t CoapPacket<OsModel_P>::type()
	{
		return type_;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_type( uint8_t type )
	{
		type_ = type;
	}

	template<typename OsModel_P>
	uint8_t CoapPacket<OsModel_P>::option_count()
	{
		return ( string_options_.size()
				+ uint_options_.size()
				+ opaque_options_.size()
				+ opt_if_none_match_ );
	}

	template<typename OsModel_P>
	uint8_t CoapPacket<OsModel_P>::code()
	{
		return code_;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_code( uint8_t code )
	{
		code_ = code;
	}

	template<typename OsModel_P>
	uint16_t CoapPacket<OsModel_P>::msg_id()
	{
		return msg_id_;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_msg_id( uint16_t msg_id )
	{
		msg_id_ = msg_id;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_data( uint8_t* data , size_t length)
	{
		data_ = data; data_length_ = length;
	}

	template<typename OsModel_P>
	uint8_t* CoapPacket<OsModel_P>::data()
	{
		return data_;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::data_length()
	{
		return data_length_;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_length()
	{
		// save all option numbers to see if fenceposting is needed (which would increase the length)
		uint32_t optnums = 0;

		// the header is 4 bytes long
		size_t length = 4;

		typename list_static<OsModel, StringOption, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		for(; sit != string_options_.end(); ++sit)
		{
			// Option header
			++length;
			if( ( *sit ).value().length() > 14 )
			{
				++length;
			}
			length += ( *sit ).value().length();
			optnums |= 1 << ( *sit ).option_number();
		}

		typename list_static<OsModel, UintOption, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		for(; uit != uint_options_.end(); ++uit)
		{
			// Option header
			++length;

			// TODO: Magic Number.... die Uint-Optionen sind in der aktuellen Version (draft-07)
			// des Standards maximal 4 byte lang (Max-Age)
			// TODO: und es gibt sicher einen eleganteren Weg herauszufinden wie viele Bytes
			// man für eine Zahl braucht
			for( int i = 0; i < 4; ++i )
			{
				if ( ( *uit ).value() % (0x100 << (i * 8)) == ( *uit ).value() )
				{
					length += i+1;
					break;
				}
			}
			optnums |= 1 << ( *uit ).option_number();
		}

		typename list_static<OsModel, OpaqueOption, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();
		for(; oit != opaque_options_.end(); ++oit)
		{
			// Option header
			++length;
			// TODO: nach aktuellem Standard (draft07) gibt es keine Opaque Options
			// länger als 8 byte, klären ob das so bleibt, oder ob man wie bei
			// String optionen damit rechnen muss, dass die Optionen länger als 14
			// byte werden können
			length += ( *oit ).length();
			optnums |= 1 << ( *oit ).option_number();
		}

		length += opt_if_none_match();

		length += data_length();

		// determine the need for fenceposts
		uint8_t previous_option_number = 0;
		for( int i = 1; i <= COAP_LARGEST_OPTION_NUMBER; ++i)
		{
			if( optnums & ( 1 << i) )
			{
				previous_option_number = i;
				continue;
			}
			// 0xf is the largest number describable by a nibble (I've always wanted to use that term :D).
			if( i - previous_option_number > 0xf )
			{
				++length;
				previous_option_number = COAP_OPT_FENCEPOST * ( 1 + (previous_option_number / (uint8_t) COAP_OPT_FENCEPOST ) );
			}
		}

		return length;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize( uint8_t *datastream )
	{
		datastream[0] = ((version() & 0x03) << 6) | ((type() & 0x03) << 4) | (option_count() & 0x0f);
		datastream[1] = code();
		datastream[2] = (this->msg_id() & 0xff00) >> 8;
		datastream[3] = (this->msg_id() & 0x00ff);

		size_t offset = COAP_START_OF_OPTIONS;

		typename list_static<OsModel, UintOption, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		typename list_static<OsModel, StringOption, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		typename list_static<OsModel, OpaqueOption, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();

		size_t sindex;
		size_t uindex;
		size_t oindex;
		uint8_t previous_option_number = 0;

		while( true )
		{
			if( uit != uint_options_.end() )
			{
				uindex = ( *uit ).option_number();
			}
			else
			{
				uindex = (size_t) - 1;
			}

			if( sit != string_options_.end() )
			{
				sindex = ( *sit ).option_number();
			}
			else
			{
				sindex = (size_t) - 1;
			}

			if( oit != opaque_options_.end() )
			{
				oindex = ( *oit ).option_number();
			}
			else
			{
				oindex = (size_t) - 1;
			}

			// all options have been serialized
			if( uindex == ( (size_t) - 1 )
					&& sindex == ( (size_t) - 1 )
					&& oindex == ( (size_t) - 1 ))
			{
				break;
			}

			if( uindex < sindex )
			{
				if( oindex < uindex )
				{
					offset += serialize_option( datastream + offset, previous_option_number, ( *oit ) );
					previous_option_number = ( *oit ).option_number();
					++oit;
					continue;
				}
				else
				{
					offset += serialize_option( datastream + offset, previous_option_number, ( *uit ) );
					previous_option_number = ( *uit ).option_number();
					++uit;
					continue;
				}
			}
			else
			{
				if( oindex < sindex )
				{
					offset += serialize_option( datastream + offset, previous_option_number, ( *oit ) );
					previous_option_number = ( *oit ).option_number();
					++oit;
					continue;
				}
				else
				{
					offset += serialize_option( datastream + offset, previous_option_number, ( *sit ) );
					previous_option_number = ( *sit ).option_number();
					++sit;
					continue;
				}
			}
		}

		if(opt_if_none_match())
		{
			fenceposting( (uint8_t) COAP_OPT_IF_NONE_MATCH, previous_option_number, datastream, offset );
			datastream[offset] = (uint8_t) (( COAP_OPT_IF_NONE_MATCH - previous_option_number ) << 4 );
			++offset;
		}

		memcpy( datastream + offset, data(), data_length());
		offset += data_length();

		return offset;
	}

	template<typename OsModel_P>
	bool CoapPacket<OsModel_P>::opt_if_none_match()
	{
		return opt_if_none_match_;
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::set_opt_if_none_match( bool opt_if_none_match )
	{
		opt_if_none_match_ = opt_if_none_match;
	}

	template<typename OsModel_P>
	uint32_t CoapPacket<OsModel_P>::what_options_are_set()
	{
		uint32_t result = 0;
		typename list_static<OsModel, UintOption, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		typename list_static<OsModel, StringOption, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		typename list_static<OsModel, OpaqueOption, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();
		for( ; uit != uint_options_.end(); ++uit )
		{
			result |= (1 << ( *uit ).option_number() );
		}

		for( ; sit != string_options_.end(); ++sit )
		{
			result |= (1 << ( *sit ).option_number() );
		}

		for( ; oit != opaque_options_.end(); ++oit )
		{
			result |= (1 << ( *oit ).option_number() );
		}
		return result;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::set_option( uint8_t option_number, uint32_t value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		UintOption uopt(option_number, value);
		set_option(uint_options_, uopt);
		return SUCCESS;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::set_option( uint8_t option_number, StaticString value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
		StringOption sopt(option_number, value);
		set_option(string_options_, sopt);
		return SUCCESS;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::set_option( uint8_t option_number, uint8_t *value, size_t length )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		OpaqueOption oopt(option_number, value);
		set_option(opaque_options_, oopt, length );
		return SUCCESS;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::add_option( uint8_t option_number, uint32_t value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		UintOption uopt(option_number, value);
		add_option(uint_options_, uopt);
		return SUCCESS;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::add_option( uint8_t option_number, StaticString value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
		StringOption sopt(option_number, value);
		add_option(string_options_, sopt);
		return SUCCESS;
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::add_option( uint8_t option_number, uint8_t *value, size_t length )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		OpaqueOption oopt(option_number, value);
		add_option(opaque_options_, oopt, length );
		return SUCCESS;
	}

	template <typename OsModel_P>
	int CoapPacket<OsModel_P>::get_option( uint8_t option_number, uint32_t &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		return get_option(option_number, value, uint_options_);
	}

	template <typename OsModel_P>
	int CoapPacket<OsModel_P>::get_option( uint8_t option_number, StaticString &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
		return get_option(option_number, value, string_options_);
	}

	template <typename OsModel_P>
	int CoapPacket<OsModel_P>::get_option( uint8_t option_number, OpaqueData &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		return get_option(option_number, value, opaque_options_);
	}

	template <typename OsModel_P>
	template <list_size_t N>
	int CoapPacket<OsModel_P>::get_options( uint8_t option_number, list_static<OsModel_P, uint32_t, N> &values )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		return get_options( option_number, values, uint_options_ );
	}

	template <typename OsModel_P>
	template <list_size_t N>
	int CoapPacket<OsModel_P>::get_options( uint8_t option_number, list_static<OsModel_P, StaticString, N> &values )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
		return get_options( option_number, values, string_options_ );
	}

	template <typename OsModel_P>
	template <list_size_t N>
	int CoapPacket<OsModel_P>::get_options( uint8_t option_number, list_static<OsModel_P, OpaqueData, N> &values )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		return get_options( option_number, values, opaque_options_ );
	}

	template<typename OsModel_P>
	int CoapPacket<OsModel_P>::remove_option( uint8_t option_number )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		switch( COAP_OPTION_FORMAT[option_number] )
		{
		case COAP_FORMAT_UINT:
			remove_option( uint_options_, option_number );
			break;
		case COAP_FORMAT_STRING:
			remove_option( string_options_, option_number );
			break;
		case COAP_FORMAT_OPAQUE:
			remove_option( opaque_options_, option_number );
			break;
		case COAP_FORMAT_NONE:
			if( option_number == COAP_OPT_IF_NONE_MATCH )
			{
				set_opt_if_none_match( false );
			}
			break;
		default:
			return ERR_UNKNOWN_OPT;
			break;
		}
		return SUCCESS;
	}

	// private methods

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::parse_message( uint8_t *datastream, size_t length )
	{
		if(length > 3)
			{
				version_ = datastream[0] >> 6;
				type_ = ( datastream[0] & 0x30 ) >> 4;
				size_t option_count = datastream[0] & 0x0f;
				code_ = datastream[1];
				msg_id_ = ( datastream[2] << 8 ) | datastream[3];

				uint8_t parsed_options = 0;
				uint8_t previous_option_number = 0;

				size_t i = COAP_START_OF_OPTIONS;
				uint8_t option_number = 0;
				size_t length_of_option = 0;
				// TODO: Überlegen ob das mit der while-Schleife so klug ist
				// bzw. aufpassen, dass man mit den ganzen i++ nicht aus den Options
				// rausläuft
				while( parsed_options < option_count && i < length )
				{
					option_number = previous_option_number + ( datastream[i] >> 4 );
					previous_option_number = option_number;
					length_of_option = datastream[i] & 0x0f;
					if( length_of_option == COAP_LONG_OPTION && i + 1 < length )
					{
						++i;
						length_of_option = datastream[i] + 15;
					}
					++i;
					if( i + length_of_option - 1 < length )
					{
						parse_option(option_number, length_of_option, datastream+i);
						++parsed_options;
						i += length_of_option;
					}
					else
					{
						// TODO: Fehlerhaftes Paket behandeln!
					}
				}

				// if there is no data we just leave it at (NULL, 0)
				if(i < length)
				{
					set_data( datastream + i, length - i );
				}

			}
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::parse_option( uint8_t option_number, uint16_t option_length, uint8_t* value)
	{
		if( option_number <= COAP_LARGEST_OPTION_NUMBER )
		{
			if( COAP_OPTION_FORMAT[option_number] == COAP_FORMAT_NONE )
			{
				if( option_number == COAP_OPT_IF_NONE_MATCH )
				{
					set_opt_if_none_match(true);
				}
				// else: do nothing
			}
			else if( COAP_OPTION_FORMAT[option_number] == COAP_FORMAT_UINT )
			{
				UintOption uint_opt(option_number, deserialize_uint(value, option_length));
				if( uint_opt.option_number() == COAP_OPT_ACCEPT )
				{
					add_option( uint_options_, uint_opt );
				}
				else
				{
					set_option( uint_options_, uint_opt );
				}
			}
			else if( COAP_OPTION_FORMAT[option_number] == COAP_FORMAT_STRING )
			{
				// TODO: Überschreitung der 270-Zeichen Grenze beachten
				StringOption str_opt( option_number, StaticString( (char*) value, option_length ) );
				if( str_opt.option_number() == COAP_OPT_URI_HOST )
				{
					set_option( string_options_, str_opt );
				}
				else
				{
					add_option( string_options_, str_opt );
				}
			}
			else if( COAP_OPTION_FORMAT[option_number] ==  COAP_FORMAT_OPAQUE )
			{
				// TODO: Überschreitung der 270-Zeichen Grenze beachten
				OpaqueOption opq_opt( option_number, value, option_length );
				if( opq_opt.option_number() == COAP_OPT_TOKEN )
				{
					set_option( opaque_options_, opq_opt );
				}
				else
				{
					add_option( opaque_options_, opq_opt );
				}
			}
		}
		else
		{
			// TODO: unbekannte Optionsnummer behandeln
		}
	}

	template<typename OsModel_P>
	inline uint8_t CoapPacket<OsModel_P>::next_fencepost(uint8_t previous_opt_number)
	{
		return ( COAP_OPT_FENCEPOST - ( (previous_opt_number) % COAP_OPT_FENCEPOST ) );
	}

	template<typename OsModel_P>
	inline void CoapPacket<OsModel_P>::fenceposting( uint8_t option_number, uint8_t &previous_opt_number, uint8_t *datastream, size_t &offset )
	{
		int fencepost_delta = 0;
		while( option_number - previous_opt_number > 15 )
		{
			fencepost_delta = next_fencepost(previous_opt_number);
			datastream[offset] =  fencepost_delta << 4;
			previous_opt_number = previous_opt_number + fencepost_delta;
			++offset;
		}
	}

	template<typename OsModel_P>
	inline void CoapPacket<OsModel_P>::optlength( size_t length, uint8_t *datastream, size_t &offset )
	{
		if( length > 14 )
		{
			datastream[offset] |= 0x0f;
			++offset;
			datastream[offset] = (length - 15) & 0xff;
			++offset;
		}
		else
		{
			datastream[offset] |= length & 0x0f ;
			++offset;
		}
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::set_option(list_static<OsModel_P, T, N> &options, T option)
	{
		typename list_static<OsModel_P, T, N>::iterator it = options.begin();
		for(;; ++it)
		{
			if(it == options.end())
			{
				options.push_back(option);
				break;
			}
			if( ( *it ).option_number() < option.option_number() )
			{
				continue;
			}
			if( ( *it ).option_number() == option.option_number() )
			{
				( *it ) = option;
				break;
			}
			if( ( *it ).option_number() > option.option_number() )
			{
				options.insert(it, option);
				break;
			}
		}
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::add_option(list_static<OsModel_P, T, N> &options, T option)
	{
		typename list_static<OsModel_P, T, N>::iterator it = options.begin();
		while(it != options.end() && ( *it ).option_number() <= option.option_number() )
		{
			++it;
		}
		options.insert(it, option);
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	int CoapPacket<OsModel_P>::get_option( uint8_t option_number, T &value, list_static<OsModel_P, T, N> &options )
	{
		typename list_static<OsModel_P, T, N>::iterator it = options.begin();
		while(it != options.end() && ( *it ).option_number() < option_number )
		{
			++it;
		}
		if(it != options.end())
		{
			value = (*it).value();
			return SUCCESS;
		}
		return ERR_OPT_NOT_SET;
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N, list_size_t M>
	int CoapPacket<OsModel_P>::get_options( uint8_t option_number, list_static<OsModel_P, T, N> &values, list_static<OsModel_P, T, M> &options )
	{
		typename list_static<OsModel_P, T, N>::iterator it = options.begin();
		while(it != options.end() && ( *it ).option_number() < option_number )
		{
			++it;
		}
		if(it == options.end())
		{
			return ERR_OPT_NOT_SET;
		}
		while(it != options.end() && ( *it ).option_number() == option_number )
		{
			values.push_back( ( *it ).value() );
			++it;
		}
		return SUCCESS;
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::remove_option(list_static<OsModel_P, T, N> &options, uint8_t option_number)
	{
		typename list_static<OsModel_P, T, N>::iterator it = options.begin();
		while(it != options.end())
		{
			if( ( *it ).option_number() == option_number )
			{
				it = options.erase(it);
				continue;
			}
			++it;
		}
	}


	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t previous_option_number, OpaqueOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );
		optlength( opt.length(), datastream, offset );
		memcpy( datastream + offset, opt.value(), opt.length());
		offset += opt.length();
		return offset;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t previous_option_number, StringOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );
		optlength( opt.value().length(), datastream, offset );

		memcpy(datastream + offset, opt.value().c_str(), opt.value().length());
		offset += opt.value().length();
		return offset;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t previous_option_number, UintOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );

		size_t length = 0;
		// the maximum size of the integer is 32bit or 4 byte
		// this loop determines how many bytes are actually needed to transmit a uint
		for( int i = 0; i < 4; ++i )
		{
			if ( opt.value() % (0x100 << (i * 8)) == opt.value() )
			{
				length += i+1;
				break;
			}
		}
		datastream[offset] |= (length & 0x0f);
		++offset;
		for( int i = length; i > 0; --i)
		{
			datastream[offset] = opt.value() >> (( ( i - 1) * 8 ) & 0xff);
			++offset;
		}

		return offset;
	}

	template <typename OsModel_P>
	uint32_t CoapPacket<OsModel_P>::deserialize_uint( uint8_t *datastream, size_t length)
	{
		uint32_t result = 0;
		for(size_t i = 0; i < length; ++i)
		{
			result = (result << 8) | datastream[i];
		}
		return result;
	}


}




#endif // COAP_PACKET_H
