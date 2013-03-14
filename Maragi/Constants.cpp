// $Id: Constants.cpp 42 2012-08-04 18:23:44Z wdlee91 $

#include "Common.h"

#include "Constants.h"

namespace Maragi
{
	namespace Constants
	{
		const wchar_t * const TITLE = MARAGI_TITLE;
		const wchar_t * const VERSION = MARAGI_VERSION;
		const wchar_t * const FULL_NAME = MARAGI_TITLE L" " MARAGI_VERSION;
		const wchar_t * const USER_AGENT = MARAGI_TITLE L"/" MARAGI_VERSION L" (Twitter Client Agent with libcurl, openssl, zlib)";
	}

	namespace Paths
	{
		const char * const PROTOCOL = "https";
		const char * const HOST = "api.twitter.com";
		const char * const REQUEST_TOKEN = "oauth/request_token";
		const char * const AUTHORIZE = "oauth/authorize";
		const char * const ACCESS_TOKEN = "oauth/access_token";
	}
}
