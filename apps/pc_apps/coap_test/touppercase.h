/*
 * touppercase.h
 *
 *  Created on: Feb 23, 2012
 *      Author: wiselib
 */

#ifndef TOUPPERCASE_H_
#define TOUPPERCASE_H_


#include <boost/algorithm/string.hpp>
#include <string>

namespace wiselib
{

template<typename CoapRadio_P>
class ToUpperCase
{
public:
	typedef CoapRadio_P Radio;
	typedef typename Radio::coap_packet_t coap_packet_t;
	typedef typename Radio::coap_packet_r coap_packet_r;
	typedef typename Radio::ReceivedMessage coap_message_t;

	ToUpperCase& operator=( const ToUpperCase &rhs )
	{
		// avoid self-assignment
		if(this != &rhs)
		{
			radio_ = rhs.radio_;
		}
		return *this;
	}

	ToUpperCase()
	{

	}

	ToUpperCase( const ToUpperCase &rhs )
	{
		*this = rhs;
	}

	~ToUpperCase()
	{

	}

	int init( Radio& radio)
	{
		radio_ = radio;
	}

	void receive_coap( typename Radio::node_id_t from, coap_message_t message )
	{
		coap_packet_r packet = message.message();
		cout << "ToUpperCase::receive_coap> Received Code"
				<< (int) packet.code() << " with "
				<< packet.data_length() << " Bytes payload\n";
		typename Radio::block_data_t *reply;
		size_t reply_length = 0;
		if(packet.data_length() > 0)
		{
			string payload = string( (char *) packet.data(), packet.data_length() );
			cout << "Payload String: " << payload;
			boost::to_upper( payload );
			cout << " converted to " << payload << "\n";
			char cstr[payload.length()];
			memcpy(cstr, payload.data(), payload.length());
			reply_length = payload.length();
			reply = (typename Radio::block_data_t *) cstr;
		}
		cout << "sending " << reply_length << " bytes in reply\n";
		radio_.reply( message, reply, reply_length );

	}

private:
	Radio radio_;
};

}
#endif /* TOUPPERCASE_H_ */
