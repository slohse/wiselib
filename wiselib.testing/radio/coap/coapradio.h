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

	struct coapreceiver
	{
		// TODO:
		// Typ des Receivers (bekommt der nur die Daten, oder das ganze Paket?)
		// Callback Pointer oder sowas
		// string der die Resource bezeichnet
	};

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
		typedef typename Radio::self_pointer_t self_pointer_t;
		
		void init( Radio& radio, Timer& timer, Debug& debug );
		int enable_radio();
		int disable_radio();
		node_id_t id ();
		int send (node_id_t receiver, size_t len, block_data_t *data );
		void receive(node_id_t from, size_t len, block_data_t * data);
		

	private:
		Radio *radio_;
		Timer *timer_;
		Debug *debug_;

		uint32_t msg_id_;

		uint32_t msg_id();
	};


// public
	template<typename OsModel_P,
		typename Radio_P,
		typename Timer_P,
		typename Debug_P>
	void CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P>::init(Radio& radio, Timer& timer, Debug& debug)
	{
		radio_ = &radio;
		timer_ = &timer;
		debug_ = &debug;

		// random initial message ID
		msg_id_ = rand();

	}




// private
	template<typename OsModel_P,
			typename Radio_P,
			typename Timer_P,
			typename Debug_P>
		uint32_t CoapRadio<OsModel_P, Radio_P, Timer_P, Debug_P>::msg_id()
		{
			return(msg_id_++);
		}

}


#endif // COAPRADIO_H