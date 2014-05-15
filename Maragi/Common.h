#pragma once

#include "Batang/Common.h"
#include "Gurigi/Common.h"

// All inclusion listing is dictionary order; Ordering is case insensitive.
// Standard C/C++ library & boost library inclusion

#include <cstring>

#include <exception>
#include <new>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/thread/mutex.hpp>

// External library inclusion

#include <curl/curl.h>
#include <openssl/sha.h>
#include <twitcurl/twitcurl.h>
#define RAPIDJSON_SSE2
#include <rapidjson/rapidjson.h>

// Windows API inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition, for INT8_MIN, INT8_MAX, ...
#include <shellapi.h>
#pragma warning(pop)

// Other common inclusion

#include "resource.h"
