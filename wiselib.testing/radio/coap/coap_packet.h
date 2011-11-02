#ifndef COAP_PACKET_H
#define COAP_PACKET_H

#include "coap.h"

namespace wiselib
{
	template<typename OsModel_P>
	class CoapPacket
	{
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
			StringOption( uint8_t option_number, string value) { set( option_number, value); }
			virtual ~StringOption() {}
			uint8_t option_number() {return option_number_;}
			StaticString value() {return value_; }
			void set( uint8_t option_number, string value ) { option_number_ = option_number; value_ = value; }

		private:
			uint8_t option_number_;
			StaticString value_;
		};

		class OpaqueOption
		{
		public:
			OpaqueOption() { length_ = 0; }
			OpaqueOption( uint8_t option_number, uint8_t* value, int length ) { set(option_number, value, length); }
			virtual ~OpaqueOption() {}
			uint8_t option_number() const {return option_number_;}
			size_t length() const { return length_; }
			uint8_t *value() { return value_; }
			uint8_t const *value() const { return value_; }
			void set( uint8_t option_number, uint8_t* op, size_t length) { option_number_ = option_number; memcpy( value_, op, length ); length_ = length; }

			int operator==(const OpaqueOption& other)
			{
				if ( length() == other.length() )
				{
					for(size_t i = 0; i < length(); ++i )
					{
						if( value()[i] != other.value()[i] )
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}

			int operator>(const OpaqueOption& other)
			{
				if ( length() > other.length() )
				{
					return true;
				}
				if ( length() == other.length() )
				{
					for(size_t i = 0; i < length(); ++i )
					{
						if( value()[i] > other.value()[i] )
						{
							return true;
						}
						if( value()[i] < other.value()[i] )
						{
							return false;
						}
					}
				}
				// this is either shorter in length, or both are equal
				return false;
			}

			int operator>=(const OpaqueOption& other)
			{
				if ( length() > other.length() )
				{
					return true;
				}
				if ( length() == other.length() )
				{
					for(size_t i = 0; i < length(); ++i )
					{
						if( value()[i] > other.value()[i] )
						{
							return true;
						}
						if( value()[i] < other.value()[i] )
						{
							return false;
						}
					}
					// they are equal
					return true;
				}
				// this is shorter in length
				return false;
			}

			int operator<(const OpaqueOption& other)
			{
				return(!(this >= other));
			}

			int operator<=(const OpaqueOption& other)
			{
				return(!(this > other));
			}


		private:
			uint8_t option_number_;
			uint8_t value_[COAP_OPT_MAXLEN_OPAQUE];
			size_t length_;
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

		void init( Debug& debug );

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
		void set_code( uint8_t code );
		uint16_t msg_id();
		void set_msg_id( uint16_t msg_id );
		void set_data( uint8_t* data , size_t length);
		uint8_t* data();
		size_t data_length();

		// methods dealing with Options
		template <typename T, list_size_t N>
		void set_option(list_static<OsModel, T, N> &options, T option);

		template <typename T, list_size_t N>
		void add_option(list_static<OsModel, T, N> &options, T option);

		template <typename T, list_size_t N>
		void remove_option(list_static<OsModel, T, N> &options, uint8_t option_number);

		bool opt_if_none_match();
		void set_opt_if_none_match( bool opt_if_none_match );

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
		inline uint8_t next_fencepost(uint8_t previous_opt_number);
		inline void fenceposting( uint8_t option_number, uint8_t last_opt_number, uint8_t *datastream, size_t &offset );
		inline void optlength( size_t length, uint8_t *datastream, size_t &offset );

		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, OpaqueOption &opt );
		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, StringOption &opt );
		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, UintOption &opt );

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
		// TODO
	}

	template<typename OsModel_P>
	void CoapPacket<OsModel_P>::init( Debug& debug )
	{
		debug_ = &debug;

		version_ = COAP_VERSION;
		// TODO: sinnvollen Default festlegen und dann ein COAP_MSG_DEFAULT_TYPE Makro anlegen oder so
		type_ = COAP_MSG_TYPE_NON;
		code_ = COAP_CODE_EMPTY;
		msg_id_ = 0;
		opt_if_none_match_ = false;

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

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::set_option(list_static<OsModel, T, N> &options, T option)
	{
		typename list_static<OsModel, T, N>::iterator it = options.begin();
		for(;; ++it)
		{
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
			if(it == options.end())
			{
				options.push_back(option);
				break;
			}
		}
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::add_option(list_static<OsModel, T, N> &options, T option)
	{
		typename list_static<OsModel, T, N>::iterator it = options.begin();
		while(it != options.end() && ( *it ).option_number() <= option.option_number() )
		{
			++it;
		}
		options.insert(it, option);
	}

	template <typename OsModel_P>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P>::remove_option(list_static<OsModel, T, N> &options, uint8_t option_number)
	{
		typename list_static<OsModel, T, N>::iterator it = options.begin();
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
	inline uint8_t CoapPacket<OsModel_P>::next_fencepost(uint8_t previous_opt_number)
	{
		return ( COAP_OPT_FENCEPOST - ( (previous_opt_number) % COAP_OPT_FENCEPOST ) );
	}

	template<typename OsModel_P>
	inline void CoapPacket<OsModel_P>::fenceposting( uint8_t option_number, uint8_t previous_opt_number, uint8_t *datastream, size_t &offset )
	{
		while( option_number - previous_opt_number > 15 )
		{
			datastream[offset] = next_fencepost(previous_opt_number) << 4;
			previous_opt_number += next_fencepost(previous_opt_number) << 4;
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

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t last_option_number, OpaqueOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), last_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - last_option_number ) << 4 );
		optlength( opt.length(), datastream, offset );
		memcpy( datastream + offset, opt.value(), opt.length());
		offset += opt.length();
		return offset;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t last_option_number, StringOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), last_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - last_option_number ) << 4 );
		optlength( opt.value().length(), datastream, offset );

		memcpy(datastream + offset, opt.value().c_str(), opt.value().length());
		offset += opt.value().length();
		return offset;
	}

	template<typename OsModel_P>
	size_t CoapPacket<OsModel_P>::serialize_option( uint8_t *datastream, uint8_t last_option_number, UintOption &opt )
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), last_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - last_option_number ) << 4 );

		size_t length = 0;
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


}




#endif // COAP_PACKET_H
