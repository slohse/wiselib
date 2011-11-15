#ifndef COAP_H
#define COAP_H



// TODO: ersetzen mit <wiselib.stable/external_interfaces/external_interface.h>
#include "external_interface/external_interface.h"
// TODO: mit was passendem ersetzen...
#include "util/pstl/static_string.h"
// TODO: memcpy
//#include <cstring>
// TODO: wiselib-äquivalent dazu
#include "util/pstl/list_static.h"
using namespace std;

// Config Tweaks
// of the uint options (content type, max age, uri port and accept) only accept can occur multiple times
// and currently (draft-07) there are only six media types. So 8 seemed a pretty good default
#define COAP_LIST_SIZE_UINT		8
// TODO: Sinnvollen Default finden
#define COAP_LIST_SIZE_STRING		16
// TODO: Sinnvollen Default finden
#define COAP_LIST_SIZE_OPAQUE		8


// Hilfsmakros

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
#define COAP_CODE_CREATED					65			// 2.01
#define COAP_CODE_DELETED					66			// 2.02
#define COAP_CODE_VALID						67			// 2.03
#define COAP_CODE_CHANGED					68			// 2.04
#define COAP_CODE_CONTENT					69			// 2.05
#define COAP_CODE_BAD_REQUEST				128			// 4.00
#define COAP_CODE_UNAUTHORIZED				129			// 4.01
#define COAP_CODE_BAD_OPTION				130			// 4.02
#define COAP_CODE_FORBIDDEN					131			// 4.03
#define COAP_CODE_NOT_FOUND					132			// 4.04
#define COAP_CODE_METHOD_NOT_ALLOWED		133			// 4.05
#define COAP_CODE_NOT_ACCEPTABLE			134			// 4.06
#define COAP_CODE_PRECONDITION_FAILED		140			// 4.12
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE	141			// 4.13
#define COAP_CODE_UNSUPPORTED_MEDIA_TYPE	143			// 4.15
#define COAP_CODE_INTERNAL_SERVER_ERROR		160			// 5.00
#define COAP_CODE_NOT_IMPLEMENTED			161			// 5.01
#define COAP_CODE_BAD_GATEWAY				162			// 5.02
#define COAP_CODE_SERVICE_UNAVAILABLE		163			// 5.03
#define COAP_CODE_GATEWAY_TIMEOUT			164			// 5.04
#define COAP_CODE_PROXYING_NOT_SUPPORTED	165			// 5.05


#define COAP_START_OF_OPTIONS	4

#define COAP_FORMAT_NONE	0
#define COAP_FORMAT_UINT	1
#define COAP_FORMAT_STRING	2
#define COAP_FORMAT_OPAQUE	3
#define COAP_LARGEST_OPTION_NUMBER	21
#define COAP_OPTION_FORMAT_ARRAY_SIZE	COAP_LARGEST_OPTION_NUMBER + 1

static const uint8_t COAP_OPTION_FORMAT[COAP_OPTION_FORMAT_ARRAY_SIZE] =
{
	COAP_FORMAT_NONE,			// 0: not in use
	COAP_FORMAT_UINT,			// 1: COAP_OPT_CONTENT_TYPE
	COAP_FORMAT_UINT,			// 2: COAP_OPT_MAX_AGE
	COAP_FORMAT_STRING,			// 3: COAP_OPT_PROXY_URI
	COAP_FORMAT_OPAQUE,			// 4: COAP_OPT_ETAG
	COAP_FORMAT_STRING,			// 5: COAP_OPT_URI_HOST
	COAP_FORMAT_STRING,			// 6: COAP_OPT_LOCATION_PATH
	COAP_FORMAT_UINT,			// 7: COAP_OPT_URI_PORT
	COAP_FORMAT_STRING,			// 8: COAP_OPT_LOCATION_QUERY
	COAP_FORMAT_STRING,			// 9: COAP_OPT_URI_PATH
	COAP_FORMAT_NONE,			// 10: not in use
	COAP_FORMAT_OPAQUE,			// 11: COAP_OPT_TOKEN
	COAP_FORMAT_UINT,			// 12: COAP_OPT_ACCEPT
	COAP_FORMAT_OPAQUE,			// 13: COAP_OPT_IF_MATCH
	COAP_FORMAT_NONE,			// 14: COAP_OPT_FENCEPOST
	COAP_FORMAT_STRING,			// 15: COAP_OPT_URI_QUERY
	COAP_FORMAT_NONE,			// 16: not in use
	COAP_FORMAT_NONE,			// 17: not in use
	COAP_FORMAT_NONE,			// 18: not in use
	COAP_FORMAT_NONE,			// 19: not in use
	COAP_FORMAT_NONE,			// 20: not in use
	COAP_FORMAT_NONE			// 21: COAP_OPT_IF_NONE_MATCH
};

#endif // COAP_H
