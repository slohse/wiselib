/*
 * coapresource.h
 *
 *  Created on: Feb 23, 2012
 *      Author: wiselib
 */

#ifndef COAPRESOURCE_H_
#define COAPRESOURCE_H_


#include <map>
#include <string>
#include <list>
#include "radio/coap/coapradio.h"

namespace wiselib
{

class CoapResource
{
public:
	CoapResource()
	{
		resource_name_ = "";
		data_ = NULL;
		data_len_ = 0;
		subresources_ = map<string, CoapResource>();
	}

	~CoapResource()
	{
		if( data_ > 0 )
		{
			free(data_);
		}
	}

	const string& resource_name() const
	{
		return resource_name_;
	}

	void add_subresource( CoapResource subresource )
	{
		if( subresource.resource_name().compare("") != 0 )
		{
			subresources_[subresource.resource_name()] = subresource;
		}
	}

	void remove_subresource( string resource )
	{
		subresources_.erase( resource );
	}

	CoapResource* subresource( list<string> subresource_path )
	{
		if( subresource_path.empty() )
			return this;
		else
		{
			map<string, CoapResource>::iterator it;
			if( ( it = subresources_.find( subresource_path.front() )) != subresources_.end() )
			{
				list<string> path_shortened(subresource_path);
				path_shortened.pop_front();
				return (*it).second.subresource( path_shortened );
			}
			else
				return NULL;
		}
	}

	void set_data(uint8_t *payload, size_t length)
	{
		if( data_ > 0 )
		{
			free(data_);
			data_ = NULL;
		}
		if( length > 0 )
		{
			data_ = (uint8_t*) malloc( length * sizeof(uint8_t) );
			memcpy ( data_, payload, length );
			data_len_ = length;
		}
	}

	uint8_t * data()
	{
		return data_;
	}

	size_t data_length()
	{
		return data_len_;
	}


private:
	string resource_name_;
	map<string, CoapResource> subresources_;
	uint8_t * data_;
	size_t data_len_;

};
}

#endif /* COAPRESOURCE_H_ */
