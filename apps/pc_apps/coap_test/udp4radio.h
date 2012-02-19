#include <netinet/in.h>

namespace wiselib
{
	template<typename OsModel_P>
	class UDP4Radio : public RadioBase<OsModel_P,
		in_addr_t,
		typename OsModel_P::size_t,
		typename OsModel_P::block_data_t>
	{
	public:
		typedef OsModel_P OsModel;

		typedef UDP4Radio<OsModel> self_type;
		typedef self_type* self_pointer_t;

		typedef in_addr_t node_id_t;
		typedef uint8_t block_data_t;
		typedef uint16_t message_id_t;

		enum SpecialNodeIds {
			BROADCAST_ADDRESS = 0xffffffff,
			NULL_NODE_ID      = 0
		};

		int enable_radio ()
		{
			return OsModel::SUCCESS;
		}

		int disable_radio ()
		{
			return OsModel::SUCCESS;
		}
		
		int send (node_id_t receiver, size_t len, block_data_t *data)
		{
			return OsModel::SUCCESS;
		}

		node_id_t id ()
		{
			return 0x80000001;
		}
	};
}
