#ifndef COAP_PACKET_H
#define COAP_PACKET_H

#include "coap.h"

namespace wiselib
{
	template<typename OsModel_P,
	         typename Radio_P = typename OsModel_P::Radio,
	         typename Debug_P = typename OsModel_P::Debug>
	class CoapPacket
	{
	public:
		typedef OsModel_P OsModel;
		typedef Radio_P Radio;
		typedef Debug_P Debug;
		
		///@name Construction / Destruction
		///@{
		CoapPacket();
		CoapPacket( uint8_t *datastream, size_t length );
		~CoapPacket();
		///@}

		void init( Radio& radio, Debug& debug )
		{
			radio_ = &radio;
			debug_ = &debug;

			version_ = COAP_VERSION;
			// TODO: sinnvollen Default festlegen und dann ein COAP_MSG_DEFAULT_TYPE Makro anlegen oder so
			type_ = COAP_MSG_TYPE_NON;
			code_ = 0;

			data_length_ = 0;
		}

	private:
		typename Radio::self_pointer_t radio_;
		typename Debug::self_pointer_t debug_;

		uint8_t version_;
		uint8_t type_;
		uint8_t code_;
		uint16_t msg_id_;

		uint8_t* data_;
		size_t data_length_;
	};

}


#endif // COAP_PACKET_H
