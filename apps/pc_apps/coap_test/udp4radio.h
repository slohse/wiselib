#ifndef UDP4RADIO_H_
#define UDP4RADIO_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include "ipv4_socket.h"

namespace wiselib
{
	template<typename OsModel_P>
	class UDP4Radio : public RadioBase<OsModel_P,
		IPv4Socket,
		typename OsModel_P::size_t,
		typename OsModel_P::block_data_t>
	{
	public:
		typedef OsModel_P OsModel;

		typedef UDP4Radio<OsModel> self_type;
		typedef self_type* self_pointer_t;

		typedef IPv4Socket node_id_t;
		typedef uint8_t block_data_t;
		typedef uint16_t message_id_t;

		enum SpecialNodeIds {
			BROADCAST_ADDRESS = 0xffffffff,
			NULL_NODE_ID      = 0
		};

		UDP4Radio() {
#ifdef DEBUG_COAPRADIO_PC
		cout << "UDP4Radio::UDP4Radio()";
#endif
			enabled = false;
			sock = 0;

			serv_addr = IPv4Socket();
		}

		void set_socket( int socket )
		{
			sock = socket;
		}

		int enable_radio ()
		{
#ifdef DEBUG_COAPRADIO_PC
		cout << "UDP4Radio::enable_radio";
#endif

			enabled = true;
			return OsModel::SUCCESS;
		}

		int disable_radio ()
		{
#ifdef DEBUG_COAPRADIO_PC
		cout << "UDP4Radio::disable_radio";
#endif
			enabled = false;
			return OsModel::SUCCESS;
		}
		
		int send (node_id_t receiver, size_t len, block_data_t *data)
		{
			cout << "UDP4Radio::send>\n";
			sockaddr_in recv_sock;
			recv_sock.sin_family = AF_INET;
			recv_sock.sin_port = receiver.port();
			recv_sock.sin_addr.s_addr = receiver.addr();

			if(enabled)
			{
				cout << "UDP4Radio::send> sending " << len << " Bytes to"
						<< ((recv_sock.sin_addr.s_addr & 0xff000000) >> 24)
						<< "."
						<< ((recv_sock.sin_addr.s_addr & 0x00ff0000) >> 16)
						<< "."
						<< ((recv_sock.sin_addr.s_addr & 0x0000ff00) >> 8)
						<< "."
						<< (recv_sock.sin_addr.s_addr & 0x000000ff)
						<< "\n";
				sendto( sock, data, len, 0, (struct sockaddr *) &recv_sock, sizeof( struct sockaddr_in ));
				return OsModel::SUCCESS;
			}
			return OsModel::ERR_UNSPEC;
		}

		node_id_t id ()
		{
			return serv_addr;
		}

	private:
		bool enabled;
		int sock;
		node_id_t serv_addr;

	};
}

#endif /* UDP4RADIO_H_ */
