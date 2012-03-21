#ifndef COAP_PACKET_H
#define COAP_PACKET_H

#include "coap.h"


#define SKIP_OPTION_AND_CONTINUE		previous_option_number = option_number; \
					i += length_of_option; \
					++parsed_options; \
					continue; \

namespace wiselib
{
	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	class CoapPacket
	{
		template<typename T>
		class CoapOption
		{
		public:
			CoapOption& operator=( const CoapOption & rhs )
			{ // avoid self-assignemnt
				if(this != &rhs)
				{
					set( rhs.option_number(), rhs.value() );
				}
				return *this;
			}
			bool operator==( const CoapOption &rhs ) const { return (this->option_number_ == rhs.option_number_ && this->value_ == rhs.value_); }
			CoapOption() { option_number_ = 0; }
			CoapOption( uint8_t option_number, const T &value) { set(option_number, value); }
			virtual ~CoapOption() {}
			uint8_t option_number() const {return option_number_;}
			const T& value() const {return value_; }
			void set( uint8_t option_number, const T &value ) { option_number_ = option_number; value_ = value; }

		private:
			uint8_t option_number_;
			T value_;
		};

	public:
		typedef OsModel_P OsModel;
		typedef typename OsModel_P::Debug Debug;
		typedef Radio_P Radio;
		typedef typename Radio::block_data_t block_data_t;
		typedef String_T string_t;

		typedef CoapPacket<OsModel_P, Radio_P, String_T> self_type;
		typedef self_type* self_pointer_t;
		typedef self_type coap_packet_t;
		typedef self_type& coap_packet_r;
		typedef self_type* coap_packet_p;

		coap_packet_t& operator=(const coap_packet_t &rhs);
		bool operator==(const coap_packet_t &rhs);

		///@name Construction / Destruction
		///@{
		CoapPacket( );
		CoapPacket( const coap_packet_t &rhs );
		~CoapPacket();
		///@}

		void init();

		/**
		 * Takes a stream of data and tries to parse it into the CoapPacket from which this method is called
		 * If parsing fails this CoapPacket will be turned into a RST-Message with the corresponding Error code, so it can be send to the sender of the offending message.
		 * @param datastream the serial data to be parsed
		 * @param length length of the datastream
		 * @return error code, 0 for success, the Coap Code corresponding to the error otherwise.
		 */
		int parse_message( block_data_t *datastream, size_t length );

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
		 * Returns the number of options that are set in this packet.
		 * @return number of options set
		 */
		uint8_t option_count() const;

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
		 * Returns the token by which request/response matching can be done.
		 * @param token reference to OpaqueData object, will contain message token afterwards
		 */
		void token( OpaqueData &token );

		/**
		 * Sets the token by which request/response matching can be done.
		 * @param token new message ID
		 */
		void set_token( const OpaqueData &token );

		string_t uri_path();

		int set_uri_path( string_t &path );

		int set_uri_query( string_t &query );

		void set_uri_port( uint32_t port );

		uint32_t uri_port();

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

		/**
		 * Calculates the length of the message if it were serialized in the current sate
		 * @return the expected length of the serialized message
		 */
		const size_t serialize_length() const;

		/**
		 * Serializes the packet so it can be sent over the radio.
		 * @param pointer to where the serialized packet will be written.
		 * @return length of the packet
		 */
		size_t serialize( block_data_t *datastream ) const;



		uint32_t what_options_are_set() const;

		int set_option( CoapOptionNum option_number, uint32_t value );
		int set_option( CoapOptionNum option_number, const string_t &value );
		int set_option( CoapOptionNum option_number, const OpaqueData &value );

		int add_option( CoapOptionNum option_number, uint32_t value );
		int add_option( CoapOptionNum option_number, const string_t &value );
		int add_option( CoapOptionNum option_number, const OpaqueData &value );

		int get_option( CoapOptionNum option_number, uint32_t &value );
		int get_option( CoapOptionNum option_number, string_t &value );
		int get_option( CoapOptionNum option_number, OpaqueData &value );

		template <list_size_t N>
		int get_options( CoapOptionNum option_number, list_static<OsModel_P, uint32_t, N> &values );

		template <list_size_t N>
		int get_options( CoapOptionNum option_number, list_static<OsModel_P, string_t, N> &values );

		template <list_size_t N>
		int get_options( CoapOptionNum option_number, list_static<OsModel_P, OpaqueData, N> &values );

		int remove_option( CoapOptionNum option_number );

		bool opt_if_none_match() const;
		void set_opt_if_none_match( bool opt_if_none_match );

		int error_response( bool needs_to_be_CON, CoapCode error_code, char *error_description, size_t len );

		enum error_code
		{
			// inherited from concepts::BasicReturnValues_concept
			SUCCESS = OsModel::SUCCESS,
			ERR_UNSPEC = OsModel::ERR_UNSPEC,
			ERR_NOTIMPL = OsModel::ERR_NOTIMPL,
			// coap specific
			ERR_WRONG_TYPE,
			ERR_UNKNOWN_OPT,
			ERR_OPT_NOT_SET,
			// message parsing errors
			ERR_CON_RESPONSE,		// error occured, requires a Confirmable response
			ERR_RST,				// error occured, requires a to respond with RST
			ERR_IGNORE_MSG			// error occured, according to RFC the entire message must be ignored
		};

	private:
		typename Debug::self_pointer_t debug_;

		uint8_t version_;
		CoapType type_;
		CoapCode code_;
		coap_msg_id_t msg_id_;

		// options
		list_static<OsModel, CoapOption<uint32_t>, COAP_LIST_SIZE_UINT> uint_options_;
		list_static<OsModel, CoapOption<string_t>, COAP_LIST_SIZE_STRING> string_options_;
		list_static<OsModel, CoapOption<OpaqueData>, COAP_LIST_SIZE_OPAQUE> opaque_options_;
		bool opt_if_none_match_;

		block_data_t data_[COAP_DATA_SIZE];
		size_t data_length_;

		// methods:
		int parse_option( uint8_t option_number, size_t option_length, block_data_t* value);
		uint8_t next_fencepost_delta(uint8_t previous_opt_number) const;
		void fenceposting( uint8_t option_number, uint8_t &previous_opt_number, block_data_t *datastream, size_t &offset ) const;
		uint8_t fenceposts_needed() const;
		void optlength( size_t length, block_data_t *datastream, size_t &offset ) const;
		bool is_critical( uint8_t option_number );

		// methods dealing with Options
		template <typename T, list_size_t N>
		void set_option( list_static<OsModel_P, CoapOption<T>, N> &options, CoapOption<T> &option );

		template <typename T, list_size_t N>
		void add_option( list_static<OsModel_P, CoapOption<T>, N> &options, CoapOption<T> &option );

		template <typename T, list_size_t N>
		int get_option( uint8_t option_number, T &value, list_static<OsModel_P, CoapOption<T>, N> &options );

		template <typename T, list_size_t N, list_size_t M>
		int get_options( uint8_t option_number, list_static<OsModel_P, T, N> &values, list_static<OsModel_P, CoapOption<T>, M> &options );

		template <typename T, list_size_t N>
		void remove_option( list_static<OsModel_P, CoapOption<T>, N> &options, uint8_t option_number );

		size_t serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<OpaqueData> &opt ) const;
		size_t serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<string_t> &opt ) const;
		size_t serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<uint32_t> &opt ) const;

		uint32_t deserialize_uint( block_data_t *datastream, size_t length);

		size_t uint_serialize_length( const uint32_t number ) const;

		int add_string_segments( char *cstr, char delimiter, CoapOptionNum optnum );

	};


// Implementation starts here
	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	CoapPacket<OsModel_P, Radio_P, String_T>& CoapPacket<OsModel_P, Radio_P, String_T>::operator=(const CoapPacket<OsModel_P, Radio_P, String_T> &rhs)
	{
		// avoid self-assignment
		if(this != &rhs)
		{
			this->init();
			this->version_ = rhs.version_;
			this->type_ = rhs.type_;
			this->code_ =  rhs.code_;
			this->msg_id_ = rhs.msg_id_;

			this->uint_options_ = rhs.uint_options_;
			this->string_options_ = rhs.string_options_;
			this->opaque_options_ = rhs.opaque_options_;
			this->opt_if_none_match_ = rhs.opt_if_none_match_;

			this->data_length_ = rhs.data_length_;
			memcpy( this->data_, rhs.data_, rhs.data_length_ );

			// doing this rather than the above would save 16 bytes
/*			block_data_t buf[rhs.serialize_length()];
			rhs.serialize(buf);
			this->parse_message(buf, rhs.serialize_length());
			*/
		}
		return *this;
	}

	template<typename OsModel_P,
			typename Radio_P,
			typename String_T>
	bool CoapPacket<OsModel_P, Radio_P, String_T>::operator==(const coap_packet_t &rhs)
	{
/*		if(rhs.version_ == this->version_
				&& rhs.type_ == this->type_
				&& rhs.code_ == this->code_
				&& rhs.msg_id_ == this->msg_id_
				&& rhs.data_length_ == this->data_length_
				&& memcmp( this->data_, rhs.data_, this->data_length_) == 0
				&& rhs.opt_if_none_match_ == this->opt_if_none_match_
				&& rhs.uint_options_.size() == this->uint_options_.size()
				&& rhs.string_options_.size() == this->string_options_.size()
				&& rhs.opaque_options_.size() == this->opaque_options_.size()
			)
		{
			typename list_static<OsModel_P, CoapOption<T>, N>::iterator it = options.begin();
			for()
			return true;
		}
		return false;
		*/
		// the dirty, el-cheapo way of doing it. Saves Codesize though... (at the expense of running time I imagine)
		if( rhs.serialize_length() == this->serialize_length() )
		{
			block_data_t buf[this->serialize_length()];
			block_data_t rbuf[this->serialize_length()];
			rhs.serialize(rbuf);
			this->serialize(buf);
			return (memcmp( buf, rbuf, this->serialize_length()) == 0);
		}
		return false;
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::parse_message( block_data_t *datastream, size_t length )
	{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> \n";
#endif
		// clear everything
		init();

		// can this possibly be a coap packet?
		if( length >= COAP_START_OF_OPTIONS )
		{
			uint8_t coap_first_byte = read<OsModel , block_data_t , uint8_t >( datastream );
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> first byte: " << hex << (int) coap_first_byte << dec << "\n";
#endif
			version_ = coap_first_byte >> 6 ;
			type_ = (CoapType) ( ( coap_first_byte & 0x30 ) >> 4 );
			size_t option_count = coap_first_byte & 0x0f;
			code_ = (CoapCode) read<OsModel , block_data_t , uint8_t >( datastream +1 );
			msg_id_ = read<OsModel , block_data_t , coap_msg_id_t >( datastream + 2 );

#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> version " << (int) version_
						<< ", type " << (int) type_
						<< ", option_count " << option_count
						<< ", code " << code_
						<< ", msg_id " << (uint16_t) msg_id_
						<<"\n";
#endif

			uint8_t parsed_options = 0;
			uint8_t previous_option_number = 0;

			size_t i = COAP_START_OF_OPTIONS;
			uint8_t option_number = 0;
			size_t length_of_option = 0;
			// TODO: Überlegen ob das mit der while-Schleife so klug ist
			// bzw. aufpassen, dass man mit den ganzen i++ nicht aus den Options
			// rausläuft
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> starting to parse options\n";
#endif
			while( parsed_options < option_count && i < length )
			{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> parsed_options: "
						<< (int) parsed_options << "/" << (int) option_count
						<< " looking at byte " << (int) i << "/" << (int) length
						<< "\n";
#endif
#ifdef DEBUG_COAPRADIO
				debug_->debug("CoapPacket::parse_message> parsed_options: %i/%i\n", parsed_options, option_count );
#endif
				option_number = previous_option_number + ( read<OsModel , block_data_t , uint8_t >( datastream + i ) >> 4 );
				length_of_option = read<OsModel , block_data_t , uint8_t >( datastream + i ) & 0x0f;
				if( length_of_option == COAP_LONG_OPTION )
				{
					if( i + 1 < length )
					{
						++i;
						length_of_option = read<OsModel , block_data_t , uint8_t >( datastream + i ) + 15;
					}
					else
					{
						if( type() == COAP_MSG_TYPE_CON )
						{
							char * error_description = NULL;
							int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
							char error_description_str[MAX_STRING_LENGTH];
							len = sprintf(error_description, "Options exceed packet length.");
							error_description = error_description_str;
#endif
							return error_response( is_request(), COAP_CODE_BAD_REQUEST, error_description, len );
						}
						else
							return ERR_IGNORE_MSG;
					}
				}

				// check for unknown options
				if ( ( option_number > COAP_LARGEST_OPTION_NUMBER ) || ( COAP_OPTION_FORMAT[option_number] == COAP_FORMAT_UNKNOWN ) )
				{
					// option is critical
					if ( is_critical( option_number ) )
					{
						if( type() == COAP_MSG_TYPE_CON )
						{
							char * error_description = NULL;
							int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
							char error_description_str[MAX_STRING_LENGTH];
							len = sprintf(error_description, "Unknown critical option %i.", option_number);
							error_description = error_description_str;
#endif
							return error_response( is_request(), COAP_CODE_BAD_OPTION , error_description, len );
						}
						else
						{
							return ERR_IGNORE_MSG;
						}

					}
					// option is not critical --> ignore
					SKIP_OPTION_AND_CONTINUE
				}
				// check for illegal repetition of options (Section 5.4.4)
				if( option_number == previous_option_number )
				{
					if ( !COAP_OPT_CAN_OCCUR_MULTIPLE[option_number] )
					{
						// option is critical
						if ( is_critical(option_number) )
						{
							if( type() == COAP_MSG_TYPE_CON )
							{
								char * error_description = NULL;
								int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
								char error_description_str[MAX_STRING_LENGTH];
								len = sprintf(error_description, "Multiple occurences of critical option %i.", option_number);
								error_description = error_description_str;
#endif
								return error_response( is_request(), COAP_CODE_BAD_OPTION , error_description, len );
							}
							else
							{
								return ERR_IGNORE_MSG;
							}
						}
						// option is not critical --> ignore
						SKIP_OPTION_AND_CONTINUE
					}
				}

				previous_option_number = option_number;
				++i;
				if( i + length_of_option - 1 < length )
				{
#ifdef DEBUG_COAPRADIO
				debug_->debug("CoapPacket::parse_message> parsing option %i, length %i\n", option_number, length_of_option );
#endif
					int option_parse_status = parse_option(option_number, length_of_option, datastream+i);
					if ( option_parse_status == SUCCESS )
					{
						++parsed_options;
						i += length_of_option;
					}
					else
					{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> parse_option returned error " << option_parse_status << "\n";
#endif
						return option_parse_status;
					}
				}
				else
				{
					if( type() == COAP_MSG_TYPE_CON )
					{
						char * error_description = NULL;
						int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
						char error_description_str[MAX_STRING_LENGTH];
						len = sprintf(error_description, "Options exceed packet length.");
						error_description = error_description_str;
#endif
						return error_response( is_request(), COAP_CODE_BAD_OPTION, error_description, len );
					}
					else
					{
						return ERR_IGNORE_MSG;
					}
				}
			}

/* TODO: OR CAN THEY?
			// responses can't have a proxy option
			uint32_t options_present = what_options_are_set();
			if( is_response() && options_present & ( 1 << COAP_OPT_PROXY_URI ))
			{

			}
			// TODO: check for other contradictory options
*/
			// if there is no data we just leave it at (NULL, 0)
			if(i < length)
			{
				set_data( datastream + i, length - i );
			}
		}
		else
		{
			// can't make any sense of it
			return ERR_IGNORE_MSG;
		}

#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_message> sucessful\n";
#endif

		return SUCCESS;
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
	uint8_t CoapPacket<OsModel_P, Radio_P, String_T>::option_count() const
	{
		return ( string_options_.size()
				+ uint_options_.size()
				+ opaque_options_.size()
				+ opt_if_none_match_ );
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
	void CoapPacket<OsModel_P, Radio_P, String_T>::token( OpaqueData &token )
	{
		if( get_option( COAP_OPT_TOKEN, token ) != SUCCESS )
		{
			token = OpaqueData();
		}
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_token( const OpaqueData &token )
	{
		remove_option( COAP_OPT_TOKEN );
		if( token != OpaqueData() )
		{
			set_option( COAP_OPT_TOKEN, token );
		}
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	String_T CoapPacket<OsModel_P, Radio_P, String_T>::uri_path()
	{
		string_t path = string_t();
		list_static<OsModel_P, string_t, COAP_LIST_SIZE_STRING> segments;
		if ( get_options( COAP_OPT_URI_PATH, segments ) == SUCCESS )
		{
			typename list_static<OsModel, string_t, COAP_LIST_SIZE_STRING>::iterator sit = segments.begin();
			path.append( (*sit) );
			++sit;
			for(; sit != segments.end(); ++sit)
			{
				path.append("/");
				path.append( (*sit) );
			}
		}
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::uri_path> path %s\n", path.c_str() );
#endif
		return path;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::set_uri_path( string_t &path )
	{
		remove_option( COAP_OPT_URI_PATH );
		char * cstr = path.c_str();
		size_t segment_start = 0;
		if( cstr[0] == '/' )
			segment_start = 1;

		return add_string_segments( cstr + segment_start , '/', COAP_OPT_URI_PATH );
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::set_uri_query( string_t &query )
	{
		remove_option( COAP_OPT_URI_QUERY );
		char * cstr = query.c_str();
		size_t segment_start = 0;
		if( cstr[segment_start] == '/' )
			++segment_start;
		if( cstr[segment_start] == '?' )
			++segment_start;

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::set_uri_query> calling add_string_segments\n");
#endif

		return add_string_segments( cstr + segment_start , '&', COAP_OPT_URI_QUERY );
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_uri_port( uint32_t port )
	{
		remove_option( COAP_OPT_URI_PORT );
		if( port != COAP_STD_PORT )
		{
			set_option( COAP_OPT_URI_PORT, port );
		}
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint32_t CoapPacket<OsModel_P, Radio_P, String_T>::uri_port()
	{
		uint32_t port = COAP_STD_PORT;
		get_option( COAP_OPT_URI_PORT, port );
		return port;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_data( block_data_t* data , size_t length)
	{
		memcpy(data_, data, length );
		data_length_ = length;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	typename Radio_P::block_data_t * CoapPacket<OsModel_P, Radio_P, String_T>::data()
	{
		return data_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	const typename Radio_P::block_data_t * CoapPacket<OsModel_P, Radio_P, String_T>::data() const
	{
		return data_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::data_length() const
	{
		return data_length_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	const size_t CoapPacket<OsModel_P, Radio_P, String_T>::serialize_length() const
	{
		// the header is 4 bytes long
		size_t length = 4;

		typename list_static<OsModel, CoapOption<string_t>, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		for(; sit != string_options_.end(); ++sit)
		{
			// Option header
			++length;
			if( ( *sit ).value().length() > 14 )
			{
				++length;
			}
			length += ( *sit ).value().length();
		}

		typename list_static<OsModel, CoapOption<uint32_t>, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		for(; uit != uint_options_.end(); ++uit)
		{
			// Option header
			++length;

			length += uint_serialize_length( ( *uit ).value() );
		}

		typename list_static<OsModel, CoapOption<OpaqueData>, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();
		for(; oit != opaque_options_.end(); ++oit)
		{
			// Option header
			++length;
			// TODO: nach aktuellem Standard (draft07) gibt es keine Opaque Options
			// länger als 8 byte, klären ob das so bleibt, oder ob man wie bei
			// String optionen damit rechnen muss, dass die Optionen länger als 14
			// byte werden können
			length += ( *oit ).value().length();
		}

		length += fenceposts_needed();

		length += opt_if_none_match();

		length += data_length();

		return length;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::serialize( block_data_t *datastream ) const
	{
		datastream[0] = ((version() & 0x03) << 6) | ((type() & 0x03) << 4) | (( option_count() + fenceposts_needed() ) & 0x0f);
		datastream[1] = code();
		datastream[2] = (this->msg_id() & 0xff00) >> 8;
		datastream[3] = (this->msg_id() & 0x00ff);

		size_t offset = COAP_START_OF_OPTIONS;

		typename list_static<OsModel, CoapOption<uint32_t>, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		typename list_static<OsModel, CoapOption<string_t>, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		typename list_static<OsModel, CoapOption<OpaqueData>, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();

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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	bool CoapPacket<OsModel_P, Radio_P, String_T>::opt_if_none_match() const
	{
		return opt_if_none_match_;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_opt_if_none_match( bool opt_if_none_match )
	{
		opt_if_none_match_ = opt_if_none_match;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint32_t CoapPacket<OsModel_P, Radio_P, String_T>::what_options_are_set() const
	{
		uint32_t result = 0;
		typename list_static<OsModel, CoapOption<uint32_t>, COAP_LIST_SIZE_UINT>::iterator uit = uint_options_.begin();
		typename list_static<OsModel, CoapOption<string_t>, COAP_LIST_SIZE_STRING>::iterator sit = string_options_.begin();
		typename list_static<OsModel, CoapOption<OpaqueData>, COAP_LIST_SIZE_OPAQUE>::iterator oit = opaque_options_.begin();
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
		if( opt_if_none_match() )
		{
			result |= (1 << COAP_OPT_IF_NONE_MATCH );
		}
		return result;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::set_option( CoapOptionNum option_number, uint32_t value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		CoapOption<uint32_t> uopt(option_number, value);
		set_option(uint_options_, uopt);
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::set_option( CoapOptionNum option_number, const string_t &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
		CoapOption<string_t> sopt(option_number, value);
		set_option(string_options_, sopt);
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::set_option( CoapOptionNum option_number, const OpaqueData &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		CoapOption<OpaqueData> oopt(option_number, value );
		set_option(opaque_options_, oopt );
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::add_option( CoapOptionNum option_number, uint32_t value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_UINT )
		{
			return ERR_WRONG_TYPE;
		}
		CoapOption<uint32_t> uopt(option_number, value);
		add_option(uint_options_, uopt);
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::add_option( CoapOptionNum option_number, const string_t &value )
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_STRING )
		{
			return ERR_WRONG_TYPE;
		}
#ifdef DEBUG_COAPRADIO
		string_t copy_of_value = value;
		debug_->debug("CoapPacket::add_option(string)> string %s\n", copy_of_value.c_str() );
#endif
		CoapOption<string_t> sopt(option_number, value);
		add_option(string_options_, sopt);
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::add_option( CoapOptionNum option_number, const OpaqueData &value)
	{
		if( option_number > COAP_LARGEST_OPTION_NUMBER )
		{
			return ERR_UNKNOWN_OPT;
		}
		if( COAP_OPTION_FORMAT[option_number] != COAP_FORMAT_OPAQUE )
		{
			return ERR_WRONG_TYPE;
		}
		CoapOption<OpaqueData> oopt(option_number, value);
		add_option(opaque_options_, oopt );
		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_option( CoapOptionNum option_number, uint32_t &value )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_option( CoapOptionNum option_number, string_t &value )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_option( CoapOptionNum option_number, OpaqueData &value )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <list_size_t N>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_options( CoapOptionNum option_number, list_static<OsModel_P, uint32_t, N> &values )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <list_size_t N>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_options( CoapOptionNum option_number, list_static<OsModel_P, string_t, N> &values )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <list_size_t N>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_options( CoapOptionNum option_number, list_static<OsModel_P, OpaqueData, N> &values )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::remove_option( CoapOptionNum option_number )
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::error_response( bool needs_to_be_CON, CoapCode error_code, char *error_description, size_t len )
	{
		OpaqueData token_tmp;
		int tokenget = get_option( COAP_OPT_TOKEN, token_tmp );
		coap_msg_id_t msg_id_tmp = msg_id();
		init();
		if( needs_to_be_CON )
		{
			set_type( COAP_MSG_TYPE_CON );
			if( tokenget == SUCCESS )
				set_option( COAP_OPT_TOKEN, token_tmp );
		}
		else
		{
			set_type( COAP_MSG_TYPE_RST );
			set_msg_id( msg_id_tmp );
		}
		set_code( error_code );
		set_data( (uint8_t*) error_description, len );
		if( needs_to_be_CON )
			return ERR_CON_RESPONSE;
		return ERR_RST;
	}

	// private methods

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::parse_option( uint8_t option_number, size_t option_length, block_data_t* value)
	{
#ifdef DEBUG_COAPRADIO_PC
				cout << "CoapPacket::parse_option> number " << (int) option_number
						<< ", length " << option_length
						<< "\n";
#endif
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
				CoapOption<uint32_t> uint_opt(option_number, deserialize_uint(value, option_length));
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
				if( option_length < COAP_STRING_OPTS_MINLEN || option_length > COAP_STRING_OPTS_MAXLEN )
				{
					// option is critical
					if ( is_critical( option_number ) )
					{
						if( type() == COAP_MSG_TYPE_CON )
						{
							char * error_description = NULL;
							int len = 0;
#if COAP_HUMAN_READABLE_ERRORS == 1
							char error_description_str[MAX_STRING_LENGTH];
							len = sprintf(error_description_str, "String option %i out of bounds.", option_number );
							error_description = error_description_str;
#endif
							return error_response( is_request(), COAP_CODE_BAD_OPTION, error_description, len );
						}
						else
						{
							return ERR_IGNORE_MSG;
						}
					}
					// option is elective --> ignore
					return ERR_IGNORE_MSG;
				}
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::parse_option> adding string option, length %i\n", option_length );
#endif
//				char str_buf[ option_length + 1];
//				memcpy( str_buf, value, option_length );
//				str_buf[option_length] = '\0';
				CoapOption<string_t> str_opt( option_number, string_t( (char*) value, option_length ) );
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::parse_option> adding string %s\n", const_cast<string_t&>(str_opt.value()).c_str() );
#endif

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
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::parse_option> adding opaque option, optnum %i\n", option_number );
#endif
				OpaqueData opq_dta( value, option_length );
				CoapOption<OpaqueData> opq_opt( option_number, opq_dta );
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

		return SUCCESS;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint8_t CoapPacket<OsModel_P, Radio_P, String_T>::next_fencepost_delta(uint8_t previous_opt_number) const
	{
		return ( COAP_OPT_FENCEPOST - ( (previous_opt_number) % COAP_OPT_FENCEPOST ) );
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::fenceposting( uint8_t option_number, uint8_t &previous_opt_number, block_data_t *datastream, size_t &offset ) const
	{
		int fencepost_delta = 0;
		while( option_number - previous_opt_number > 15 )
		{
			fencepost_delta = next_fencepost_delta(previous_opt_number);
			datastream[offset] =  fencepost_delta << 4;
			previous_opt_number = previous_opt_number + fencepost_delta;
			++offset;
		}
	}

	template<typename OsModel_P,
			typename Radio_P,
		typename String_T>
	uint8_t CoapPacket<OsModel_P, Radio_P, String_T>::fenceposts_needed() const
	{
		uint8_t num_fenceposts = 0;
		uint32_t optnums = what_options_are_set();
		uint8_t next_option_number = 0;
		for( int i = COAP_LARGEST_OPTION_NUMBER; i > 0 ; --i)
		{
			if( optnums & ( 1 << i) )
			{
				next_option_number = i;
				continue;
			}
			// 0xf is the largest number describable by a nibble (I've always wanted to use that term :D).
			if( (next_option_number > 0 ) && ( ( next_option_number - i ) >= 0xf ) )
			{
				++num_fenceposts;
				next_option_number = COAP_OPT_FENCEPOST * ( (uint8_t) ( next_option_number / (uint8_t) COAP_OPT_FENCEPOST ) );
			}
		}
		return num_fenceposts;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	void CoapPacket<OsModel_P, Radio_P, String_T>::optlength( size_t length, block_data_t *datastream, size_t &offset ) const
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	bool CoapPacket<OsModel_P, Radio_P, String_T>::is_critical( uint8_t option_number )
	{
		// odd option numbers are critical
		return( option_number & 0x01 );
	}

	// TODO: die Listen könnten voll sein, hier und in aufrufenden Funktionen abfangen!
	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P, Radio_P, String_T>::set_option(list_static<OsModel_P, CoapOption<T>, N> &options, CoapOption<T> &option)
	{
		remove_option( options, option.option_number() );
		add_option( options, option );
	}

	// TODO: die Listen könnten voll sein, hier und in aufrufenden Funktionen abfangen
	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P, Radio_P, String_T>::add_option(list_static<OsModel_P, CoapOption<T>, N> &options, CoapOption<T> &option)
	{
		typename list_static<OsModel_P, CoapOption<T>, N>::iterator it = options.begin();
		while(it != options.end() && ( *it ).option_number() <= option.option_number() )
		{
			++it;
		}
		options.insert(it, option);
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <typename T, list_size_t N>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_option( uint8_t option_number, T &value, list_static<OsModel_P, CoapOption<T>, N> &options )
	{
		typename list_static<OsModel_P, CoapOption<T>, N>::iterator it = options.begin();
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <typename T, list_size_t N, list_size_t M>
	int CoapPacket<OsModel_P, Radio_P, String_T>::get_options( uint8_t option_number, list_static<OsModel_P, T, N> &values, list_static<OsModel_P, CoapOption<T>, M> &options )
	{
		typename list_static<OsModel_P, CoapOption<T>, N>::iterator it = options.begin();
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

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	template <typename T, list_size_t N>
	void CoapPacket<OsModel_P, Radio_P, String_T>::remove_option(list_static<OsModel_P, CoapOption<T>, N> &options, uint8_t option_number)
	{
		typename list_static<OsModel_P, CoapOption<T>, N>::iterator it = options.begin();
		while( it != options.end() )
		{
			if( ( *it ).option_number() == option_number )
			{
				options.erase( it++ );
				continue;
			}
			++it;
		}
	}


	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<OpaqueData> &opt ) const
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );
		optlength( opt.value().length(), datastream, offset );
		memcpy( datastream + offset, opt.value().value(), opt.value().length());
		offset += opt.value().length();
		return offset;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<string_t> &opt ) const
	{
		size_t offset = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );
		datastream[offset] = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );
		optlength( opt.value().length(), datastream, offset );

		memcpy(datastream + offset, const_cast<string_t&>(opt.value()).c_str(), opt.value().length());
		offset += opt.value().length();
		return offset;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::serialize_option( block_data_t *datastream, uint8_t previous_option_number, CoapOption<uint32_t> &opt ) const
	{
		size_t offset = 0;
		uint8_t delta_and_size = 0;
		fenceposting( opt.option_number(), previous_option_number, datastream, offset );

		delta_and_size = (uint8_t) (( opt.option_number() - previous_option_number ) << 4 );

		size_t length = uint_serialize_length( opt.value() );
		delta_and_size |= (length & 0x0f);
		write<OsModel , block_data_t , uint8_t >( ( block_data_t* ) datastream + offset, delta_and_size);
		++offset;
		// The network byte order is big endian. and according to
		// http://stackoverflow.com/questions/1041554/bitwise-operators-and-endianness
		// the bitwise operator abstract endianness. So.... this should work... I think
		for( int i = length; i > 0; --i)
		{
			datastream[offset] = opt.value() >> (( ( i - 1) * 8 ) & 0xff);
			++offset;
		}

		return offset;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	uint32_t CoapPacket<OsModel_P, Radio_P, String_T>::deserialize_uint( block_data_t *datastream, size_t length)
	{
		uint32_t result = 0;
		// same as in the serialize_option for uints: because bitwise operators abstract endianness away, this should work,
		// even thoug I'm not using read from utils/serialization
		for(size_t i = 0; i < length; ++i)
		{
			result = (result << 8) | datastream[i];
		}
		return result;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	size_t CoapPacket<OsModel_P, Radio_P, String_T>::uint_serialize_length( const uint32_t number ) const
	{
		int i = 0;
		for( ; i < 5; ++i )
		{
			if ( number < (uint32_t) (0x1 << (i * 8)) )
			{
				break;
			}
		}
		return i;
	}

	template<typename OsModel_P,
		typename Radio_P,
		typename String_T>
	int CoapPacket<OsModel_P, Radio_P, String_T>::add_string_segments( char *cstr, char delimiter, CoapOptionNum optnum )
	{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::add_string_segments> string %s, delimiter %c \n", cstr, delimiter );
#endif

		size_t position = 0;
		size_t segment_start = 0;
		int segments = 0;
		size_t length = 0;
		while( cstr[position] != '\0' )
		{
			if( cstr[position] == delimiter )
			{
				if( (length = position - segment_start ) == 0)
					return -1;

#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::add_string_segments> found segment at %i length %i \n", segment_start, length);
#endif
//				char buffer[length + 1];
//				memcpy( buffer, cstr + segment_start, length );
//				buffer[length] = '\0';
				string_t buffer_str( cstr + segment_start, length );
				add_option( optnum , buffer_str );
				++segments;
				segment_start = position + 1;
			}
			++position;
		}
		if( position > 0 && (length = position - segment_start ) > 0)
		{
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::add_string_segments> found segment at %i length %i \n", segment_start, length);
#endif
//			char buffer[length + 1];
//			memcpy( buffer, cstr + segment_start, length );
//			buffer[length] = '\0';
			string_t buffer_str( cstr + segment_start, length );
#ifdef DEBUG_COAPRADIO
		debug_->debug("CoapPacket::add_string_segments> adding %s \n", buffer_str.c_str() );
#endif
			add_option( optnum , buffer_str );
			++segments;
		}

		return segments;
	}


}




#endif // COAP_PACKET_H
