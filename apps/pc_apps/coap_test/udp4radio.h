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
			enabled = false;
			sock = 0;

			serv_addr = IPv4Socket();
		}

		int enable_radio ()
		{
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{
				perror("socket");
				return OsModel::ERR_UNSPEC;
			}
			enabled = true;
			return OsModel::SUCCESS;
		}

		int disable_radio ()
		{
			close(sock);
			enabled = false;
			return OsModel::SUCCESS;
		}
		
		int send (node_id_t receiver, size_t len, block_data_t *data)
		{
			sockaddr_in recv_sock;
			recv_sock.sin_family = AF_INET;
			recv_sock.sin_port = receiver.port();
			recv_sock.sin_addr.s_addr = receiver.addr();
			printf("UDP4Radio::send> sending %i Bytes to %i.%i.%i.%i\n",
					len,
					(recv_sock.sin_addr.s_addr & 0xff000000) >> 24,
					(recv_sock.sin_addr.s_addr & 0x00ff0000) >> 16,
					(recv_sock.sin_addr.s_addr & 0x0000ff00) >> 8,
					(recv_sock.sin_addr.s_addr & 0x000000ff)
					);
			if(enabled)
			{
				sendto( sock, data, len, 0, (struct sockaddr *) &recv_sock, sizeof( struct sockaddr_in ));
				return OsModel::SUCCESS;
			}
			return OsModel::ERR_UNSPEC;
		}

		node_id_t id ()
		{
			return serv_addr;
		}

//		int register_client ( sockaddr_in *client )
//		{
//			if ( resources_.empty() )
//				resources_.assign( COAPRADIO_RESOURCES_SIZE, CoapResource() );
//
//			for ( unsigned int i = 0; i < resources_.size(); ++i )
//			{
//				if ( resources_.at(i) == CoapResource() )
//				{
//					resources_.at(i).set_resource_path( resource_path );
//					resources_.at(i).set_callback( coapreceiver_delegate_t::template from_method<T, TMethod>( callback ) );
//					return i;
//				}
//			}
//
//			return -1;
//		}

	private:
		bool enabled;
		int sock;
		node_id_t serv_addr;

	};
}

#endif /* UDP4RADIO_H_ */
