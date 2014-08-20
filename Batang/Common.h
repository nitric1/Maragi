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
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/assert.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/operators.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4503) // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include <boost/signals2.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#include <boost/thread/once.hpp>
#include <boost/thread/tss.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/filesystem.hpp>

using std::max; using std::min;

// External library inclusion

#if defined(_WIN32)
// Windows API inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition, for INT8_MIN, INT8_MAX, ...

#include <intrin.h>
#endif // _MSC_VER

#include <process.h>
#include <windowsx.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif

// Other common inclusion

#include "Types.h"
