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
};



#endif /* UNIT_TEST_RADIO_H_ */
