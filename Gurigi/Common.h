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

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <thread>
#include <unordered_set>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#pragma warning(push)
#pragma warning(disable: 4819) // The file contains a character that cannot be represented in the current code page
#include <boost/format.hpp>
#pragma warning(pop)

#include <boost/range/any_range.hpp>
#include <boost/range/sub_range.hpp>

#pragma warning(push)
#pragma warning(disable: 4503) // decorated name length exceeded, name was truncated
#include <boost/signals2.hpp>
#pragma warning(pop)

#include <boost/thread/once.hpp>
#include <boost/thread/tss.hpp>

using std::max; using std::min;

// External library inclusion

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

#include "Batang/Types.h"
