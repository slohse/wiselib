#ifndef COAPRADIO_H
#define COAPRADIO_H

#include "coap.h"
#include "coap_packet.h"

namespace wiselib {


template<typename OsModel_P,
	typename Radio_P,
	typename Timer_P,
	typename Debug_P>
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

		typedef typename Radio::node_id_t node_id_t;
		typedef typename Radio::size_t size_t;
		typedef typename Radio::block_data_t block_data_t;
		typedef typename Radio::message_id_t message_id_t;
		
		int enable_radio();
		int disable_radio();
		node_id_t id ();
		int send (node_id_t receiver, size_t len, block_data_t *data );
		
		
	};
}


#endif // COAPRADIO_H
