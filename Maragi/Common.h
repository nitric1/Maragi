#pragma once

#define NOMINMAX
#define _VARIADIC_MAX 10
#define _SCL_SECURE_NO_WARNINGS

#define WINVER 0x0600 // Windows Vista
#define _WIN32_WINDOWS 0x0600
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0800
#define NTDDI_VERSION 0x06000100

// All inclusion listing is dictionary order; Ordering is case insensitive.
// Standard C/C++ library & boost library inclusion

#include <cassert>
#include <cstring>

#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <new>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/tss.hpp>

using std::max; using std::min;

// External library inclusion

#include <curl/curl.h>
#include <json/json.h>
#include <openssl/sha.h>

// Windows API inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition, for INT8_MIN, INT8_MAX, ...
#include <commctrl.h>
#include <commdlg.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dlgs.h>
#include <dwrite.h>
#include <intrin.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <wincrypt.h>
#include <windowsx.h>
#pragma warning(pop)

// Other common inclusion

#include "resource.h"
#include "Types.h"
