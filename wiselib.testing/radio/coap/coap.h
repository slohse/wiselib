#ifndef COAP_H
#define COAP_H



// TODO: ersetzen mit <wiselib.stable/external_interfaces/external_interface.h>
#include <external_interfaces/external_interface.h>
// TODO: mit was passendem ersetzen...
#include <static_string.h>
// TODO: memcpy
//#include <cstring>
// TODO: wiselib-äquivalent dazu
#include <list_static>
using namespace std;

// Hilfsmakros
#define COAP_FORMAT_UINT	0
#define COAP_FORMAT_STRING	1
#define COAP_FORMAT_OPAQUE	2

// Constants defined in draft-ietf-core-coap-07
#define COAP_VERSION	1
#define COAP_STD_PORT	5683

#define COAP_MSG_TYPE_CON	0
#define COAP_MSG_TYPE_NON	1
#define COAP_MSG_TYPE_ACK	2
#define COAP_MSG_TYPE_RST	3

#define COAP_LONG_OPTION	15

#define COAP_OPT_FENCEPOST			14
#define COAP_OPT_CONTENT_TYPE		1
#define COAP_OPT_MAX_AGE			2
#define COAP_OPT_PROXY_URI			3
#define COAP_OPT_ETAG				4
#define COAP_OPT_URI_HOST			5
#define COAP_OPT_LOCATION_PATH		6
#define COAP_OPT_URI_PORT			7
#define COAP_OPT_LOCATION_QUERY		8
#define COAP_OPT_URI_PATH			9
#define COAP_OPT_TOKEN				11
#define COAP_OPT_ACCEPT				12
#define COAP_OPT_IF_MATCH			13
#define COAP_OPT_URI_QUERY			15
#define COAP_OPT_IF_NONE_MATCH		21

#define COAP_OPT_MAXLEN_FENCEPOST		0
#define COAP_OPT_MAXLEN_CONTENT_TYPE	2
#define COAP_OPT_MAXLEN_MAX_AGE			4
#define COAP_OPT_MAXLEN_PROXY_URI		270
#define COAP_OPT_MAXLEN_ETAG			8
#define COAP_OPT_MAXLEN_URI_HOST		270
#define COAP_OPT_MAXLEN_LOCATION_PATH	270
#define COAP_OPT_MAXLEN_URI_PORT		2
#define COAP_OPT_MAXLEN_LOCATION_QUERY	270
#define COAP_OPT_MAXLEN_URI_PATH		270
#define COAP_OPT_MAXLEN_TOKEN			8
#define COAP_OPT_MAXLEN_ACCEPT			2
#define COAP_OPT_MAXLEN_IF_MATCH		8
#define COAP_OPT_MAXLEN_URI_QUERY		270
#define COAP_OPT_MAXLEN_IF_NONE_MATCH	0

#define COAP_DEFAULT_MAX_AGE		60


// Finding the longest opaque option, out of the three opage options Etag, Token and IfMatch
#if COAP_OPT_MAXLEN_ETAG > COAP_OPT_MAXLEN_TOKEN
	#if COAP_OPT_MAXLEN_ETAG > COAP_OPT_MAXLEN_IF_MATCH
		#define COAP_OPT_MAXLEN_OPAQUE	COAP_OPT_MAXLEN_ETAG
	#else
		#define COAP_OPT_MAXLEN_OPAQUE	COAP_OPT_MAXLEN_IF_MATCH
	#endif
#else
	#define COAP_OPT_MAXLEN_OPAQUE	COAP_OPT_MAXLEN_TOKEN
#endif

// message codes
#define COAP_CODE_EMPTY			0
//requests
#define COAP_CODE_GET			1
#define COAP_CODE_POST			2
#define COAP_CODE_PUT			3
#define COAP_CODE_DELETE		4
//responses
#define COAP_CODE_CREATED					65
#define COAP_CODE_DELETED					66
#define COAP_CODE_VALID						67
#define COAP_CODE_CHANGED					68
#define COAP_CODE_CONTENT					69
#define COAP_CODE_BAD_REQUEST				128
#define COAP_CODE_UNAUTHORIZED				129
#define COAP_CODE_BAD_OPTION				130
#define COAP_CODE_FORBIDDEN					131
#define COAP_CODE_NOT_FOUND					132
#define COAP_CODE_METHOD_NOT_ALLOWED		133
#define COAP_CODE_PRECONDITION_FAILED		140
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE	141
#define COAP_CODE_UNSUPPORTED_MEDIA_TYPE	143
#define COAP_CODE_INTERNAL_SERVER_ERROR		160
#define COAP_CODE_NOT_IMPLEMENTED			161
#define COAP_CODE_BAD_GATEWAY				162
#define COAP_CODE_SERVICE_UNAVAILABLE		163
#define COAP_CODE_GATEWAY_TIMEOUT			164
#define COAP_CODE_PROXYING_NOT_SUPPORTED	165


#define NEXT_FENCEPOST(previous_option_number)	( COAP_OPT_FENCEPOST - ( (previous_option_number) % COAP_OPT_FENCEPOST ) )


#endif // COAP_H
