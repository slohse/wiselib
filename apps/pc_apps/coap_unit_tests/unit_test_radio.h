/*
 * unit_test_radio.h
 *
 *  Created on: Feb 24, 2012
 *      Author: wiselib
 */

#ifndef UNIT_TEST_RADIO_H_
#define UNIT_TEST_RADIO_H_


class UnitTestRadio
{
public:
	typedef uint8_t block_data_t;
	typedef uint16_t node_id_t;
	typedef uint16_t size_t;
	typedef uint8_t message_id_t;

	enum ErrorCodes
	{
		SUCCESS			= 0
	};


	class Message
	{
	public:

		Message& operator=( const Message & rhs )
		{
			if( this != &rhs )
			{
				init();
				set( rhs.receiver_, rhs.len_,rhs.data_ );
			}
			return *this;
		}

		Message()
		{
			init();
		}

		Message( node_id_t id, size_t len, block_data_t *data )
		{
			init();
			set( id, len, data );
		}

		Message( const Message & rhs )
		{
			init();
			set( rhs.receiver_, rhs.len_, rhs.data_ );
		}

		~Message()
		{
			if( data_ != NULL )
				free(data_);
		}

		void init()
		{
			receiver_ = 0;
			len_ = 0;
			data_ = NULL;
		}

		void set( node_id_t id, size_t len, block_data_t *data )
		{
			receiver_ = id;
			len_ = len;
			if( data_ != NULL )
			{
				block_data_t * new_data =
						(block_data_t*) realloc( data_,
								len_ * sizeof(block_data_t));
				if( new_data != NULL )
					data_ = new_data;
				else
				{
					cerr << "Error Allocating memory\n";
					free(data_);
					exit(1);
				}
			}
			else
			{
				data_ = (block_data_t*) malloc( len_ * sizeof(block_data_t) );
				if(data_ == NULL)
				{
					cerr << "Error Allocating memory\n";
					exit(1);
				}
			}
			memcpy( data_, data, len_ );
		}

		void get( node_id_t &id, size_t &len, block_data_t *&data )
		{
			id = receiver_;
			len = len_;
			data = data_;
		}

		block_data_t * get( node_id_t &id, size_t &len )
		{
			id = receiver_;
			len = len_;
			return data_;
		}

	private:
		node_id_t receiver_;
		size_t len_;
		block_data_t * data_;

	};

	UnitTestRadio& operator=( const UnitTestRadio &rhs )
	{
		return *this;
	}

	UnitTestRadio()
	{
	}

	UnitTestRadio( const UnitTestRadio &rhs )
	{
		*this = rhs;
	}


	~UnitTestRadio()
	{

	}

	int send( node_id_t id, size_t len, block_data_t *data )
	{
		cout << "Sending message to " << (int) id << " size " << (int) len << "\n";
		Message msg(id, len, data );
		sent_.push_back( msg );
		return SUCCESS;
	}

	int enable_radio()
	{
		return SUCCESS;
	}

	int disable_radio()
	{
		return SUCCESS;
	}

	node_id_t id()
	{
		return 42;
	}

	template<class T, void (T::*TMethod)(node_id_t, size_t, block_data_t*)>
	int reg_recv_callback(T *obj_pnt )
	{
		return 1;
	}

	void unreg_recv_callback(int idx )
	{
		//do nothing
		return;
	}

	size_t sentMessages()
	{
		return sent_.size();
	}

	Message& lastMessage()
	{
		return sent_.back();
	}

	list<Message> sent_;
};



#endif /* UNIT_TEST_RADIO_H_ */
