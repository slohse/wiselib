#ifndef COAPPACKET_H
#define COAPPACKET_H

#include "coap.h"


#define COAP_START_OF_OPTIONS	4

#define COAP_ERROR_OPTION_LENGTH	1

#define FENCEPOSTING(index)		while( (index) - last_option_number > 15 ) \
				{ \
					datastream[offset] = NEXT_FENCEPOST( last_option_number ) << 4; \
					last_option_number += NEXT_FENCEPOST( last_option_number ); \
					++offset; \
				} \

#define OPTLENGTH(len)	if( (len) > 14 ) \
			{ \
				datastream[offset] |= 0x0f; \
				++offset; \
				datastream[offset] = (((len) - 15) & 0xff ); \
				++offset; \
			} \
			else \
			{ \
				datastream[offset] |= ((len) & 0x0f ); \
				++offset; \
			} \



class CoapPacket
{
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

/*			int operator<(const OpaqueOption& other)
			{
				return(!(this >= other));
			}

			int operator<=(const OpaqueOption& other)
			{
				return(!(this > other));
			}
*/

		private:
			uint8_t option_number_;
			uint8_t value_[COAP_OPT_MAXLEN_OPAQUE];
			size_t length_;
	};

// TODO: String und UintOption zu Option<T> zusammenfassen.
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
			StringOption() { set(0, string("")); }
			StringOption( uint8_t option_number, string value) { set( option_number, value); }
			virtual ~StringOption() {}
			uint8_t option_number() {return option_number_;}
			string value() {return value_; }
			void set( uint8_t option_number, string value ) { option_number_ = option_number; value_ = value; }

		private:
			uint8_t option_number_;
			string value_;
	};

	public:
		CoapPacket();
		CoapPacket( uint8_t* datastream, size_t length );

		size_t serialize(uint8_t *datastream);
		size_t serialize(uint8_t *datastream, uint16_t msg_id);
		size_t serialize_length();
#ifdef DEBUG
		string vectorToString(vector<string> strv);
		string optionsToString();
		string toString();
#endif
		virtual ~CoapPacket();
		uint8_t version() { return version_; }
		void set_version( uint8_t version ) { version_ = version; }
		uint8_t type() { return type_; }
		void set_type( uint8_t type ) { type_ = type; }
		uint8_t option_count() { return ( string_options_.size() + uint_options_.size() + opaque_options_.size() + opt_if_none_match_ ); }
		uint8_t code() { return code_; }
		void set_code( uint8_t code ) { code_ = code; }
		uint16_t msg_id() { return msg_id_; }
		void set_msg_id( uint16_t msg_id ) { msg_id_ = msg_id; }
		void set_data( uint8_t* data , size_t length) { data_ = data; data_length_ = length; }
		uint8_t* data() { return data_; }
		size_t data_length() { return data_length_; }


		// functions for manipulating options
		bool opt_content_type(uint16_t &content_type);
		void set_opt_content_type( uint16_t content_type );

		bool opt_max_age( uint32_t &max_age );
		void set_opt_max_age( uint32_t max_age );

		bool opt_proxy_uri( list<string> &proxy_uri );
		void add_opt_proxy_uri( string proxy_uri );
		void set_opt_proxy_uri( list<string> proxy_uri );
		void remove_opt_proxy_uri ();

		bool opt_etag(uint8_t *etag, size_t length);
		void set_opt_etag(uint8_t *etag, size_t length);
		void remove_opt_etag();

		bool opt_uri_host( string &uri_host);
		void set_opt_uri_host( string uri_host );

		bool opt_location_path( list<string> &location_path );
		void add_opt_location_path( string location_path );
		void set_opt_location_path( list<string> location_path );
		void remove_opt_location_path ();

		bool opt_uri_port(uint16_t & uri_port);
		void set_opt_uri_port( uint16_t uri_port );

		bool opt_location_query( list<string> &location_query );
		void add_opt_location_query( string location_query );
		void set_opt_location_query( list<string> location_query );
		void remove_opt_location_query ();

		bool opt_uri_path( list<string> &uri_path );
		void add_opt_uri_path( string uri_path );
		void set_opt_uri_path( list<string> uri_path );
		void remove_opt_uri_path ();

		bool opt_token(uint8_t *token, size_t length);
		void set_opt_token(uint8_t *token, size_t length);
		void remove_opt_token();

		bool opt_accept( list<uint16_t> &accept );
		void add_opt_accept( uint16_t accept );
		void set_opt_accept( list<uint16_t> accept );
		void remove_opt_accept ();
		void remove_opt_accept ( uint16_t accept );

		bool opt_if_match( list<OpaqueOption> &if_match );
		void add_opt_if_match( uint8_t *value, size_t length );
		void remove_opt_if_match( OpaqueOption if_match );

		bool opt_uri_query( list<string> &uri_query );
		void add_opt_uri_query( string uri_query );
		void set_opt_uri_query( list<string> uri_query );
		void remove_opt_uri_query ();
/*
		vector<string> &opt_uri_query() { return opt_uri_query_; }
		void set_opt_uri_query( vector<string> uri_query ) { opt_uri_query_ = uri_query; }
		void add_opt_uri_query( string uri_query ) { opt_uri_query_.push_back( uri_query ); }
		void remove_opt_uri_query ( string uri_query ) { remove_string(opt_uri_query_, uri_query ); }
*/
		bool opt_if_none_match() { return opt_if_none_match_; }
		void set_opt_if_none_match( bool opt_if_none_match ) { opt_if_none_match_ = opt_if_none_match; }


	protected:
	private:
		uint8_t version_;
		uint8_t type_;
		uint8_t code_;
		uint16_t msg_id_;

		// options.
		list<UintOption> uint_options_;
		list<StringOption> string_options_;
		list<OpaqueOption> opaque_options_;
		bool opt_if_none_match_;

		// data
		uint8_t* data_;
		size_t data_length_;

		// internal helper functions
		void init();
		void parse_option( uint8_t option_number, uint16_t option_length, uint8_t* value);

		size_t get_option(uint8_t option_number, uint32_t &value)
		{
			UintOption opt;
			size_t num = get_option(uint_options_, option_number, opt);
			if(num > 0)
			{
				value = opt.value();
			}
			return num;
		}

		size_t get_option(uint8_t option_number, string &value)
		{
			StringOption opt;
			size_t num = get_option(string_options_, option_number, opt);
			if(num > 0)
			{
				value = opt.value();
			}
			return num;
		}

		size_t get_option(uint8_t option_number, uint8_t *value, size_t length)
		{
			OpaqueOption opt;
			size_t num = get_option(opaque_options_, option_number, opt);
			if(num > 0)
			{
				value = opt.value();
				length = opt.length();
			}
			return num;
		}

		template <typename T>
		size_t get_option(list<T> &options, uint8_t option_number, T &value)
		{
			size_t number_found = 0;
			typename list<T>::iterator it = options.begin();
			for(; it != options.end(); ++it)
			{
				if( ( *it ).option_number() == option_number )
				{
					if(number_found == 0)
					{
						value = (* it );
					}
					++number_found;
				}
			}
			return number_found;
		}

		template <typename T>
		bool get_option_all(list<T> &options, uint8_t option_number, list<T> &value)
		{
			typename list<T>::iterator it = options.begin();
			typename list<T>::iterator start = options.end();
			typename list<T>::iterator end = options.end();
			for(; it != options.end(); ++it)
			{
				if( ( *it ).option_number() == option_number && start == options.end() )
				{
					start = it;
				}
				if( ( *it ).option_number() > option_number && start != options.end() )
				{
					end = it;
				}
			}

			if( start != options.end() )
			{
				value.clear();
				value.insert( value.begin(), start, end );
				return true;
			}
			return false;
		}

		bool get_option_all(uint8_t option_number, list<string> &values)
		{
			list<StringOption> vals;
			bool status = get_option_all(string_options_, option_number, vals);
			if(status)
			{
				values.clear();
				list<StringOption>::iterator it = vals.begin();
				for(; it != vals.end(); ++it)
				{
					values.push_back( ( *it ).value() );
				}
				return true;
			}
			return false;
		}

		// sets an option
		// resets the first instance of the option it finds in the list, otherwise it inserts a new option
		// options sorted by their option_number
		template <typename T>
		void set_option(list<T> &options, T option)
		{
			typename list<T>::iterator it = options.begin();
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

		template <typename T>
		void set_option(list<T> &options, list<T> option)
		{
			uint8_t option_number = ( * option.begin() ).option_number();
			remove_option(options, option_number);

			typename list<T>::iterator it = options.begin();
			while( it != options.end() && ( *it ).option_number() <= option_number )
			{
				++it;
			}
			options.insert( it, option.begin(), option.end() );
		}

		// TODO: das funktioniert doch nie im Leben! Variablenscopes...?
		void set_option(uint8_t option_number, list<string> &options)
		{
			list<StringOption> opts;

			list<string>::iterator it = options.begin();
			StringOption ins;
			for(; it != options.end(); ++it)
			{
				ins.set( option_number, ( *it ) );
				opts.push_back( ins );
			}
			set_option(string_options_, opts);
		}

		template <typename T>
		void add_option(list<T> &options, T option)
		{
			typename list<T>::iterator it = options.begin();
			while(it != options.end() && ( *it ).option_number() <= option.option_number() )
			{
				++it;
			}
			options.insert(it, option);
		}

		template <typename T>
		void remove_option(list<T> &options, uint8_t option_number)
		{
			typename list<T>::iterator it = options.begin();
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

		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, OpaqueOption &opt )
		{
			size_t offset = 0;
			FENCEPOSTING( opt.option_number() )
			datastream[offset] = (uint8_t) (( opt.option_number() - last_option_number ) << 4 );
			OPTLENGTH(opt.length())
			memcpy( datastream + offset, opt.value(), opt.length());
			offset += opt.length();
			return offset;
		}

		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, StringOption &opt )
		{
			size_t offset = 0;
			FENCEPOSTING( opt.option_number() )
			datastream[offset] = (uint8_t) (( opt.option_number() - last_option_number ) << 4 );
			OPTLENGTH(opt.value().length())

			memcpy(datastream + offset, opt.value().c_str(), opt.value().length());
			offset += opt.value().length();
			return offset;
		}

		size_t serialize_option( uint8_t *datastream, uint8_t last_option_number, UintOption &opt )
		{
			size_t offset = 0;
			FENCEPOSTING( opt.option_number() )
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
};

#endif // COAPPACKET_H

