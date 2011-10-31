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
	};


// Implementation starts here

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



}




#endif // COAP_PACKET_H
