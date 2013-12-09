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
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/tss.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/filesystem.hpp>

using std::max; using std::min;

// External library inclusion

#if defined(_WIN32)
// Windows API inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN

#define BATANG_TIMER_WIN32

#include <windows.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition, for INT8_MIN, INT8_MAX, ...

#include <intrin.h>
#endif // _MSC_VER

#include <windowsx.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#elif defined(__linux__)
// Linux API inclusion; <linux/version.h> should be first.

#include <linux/version.h>

#include <time.h>
#include <unistd.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#define BATANG_TIMER_TIMERFD

#include <sys/timerfd.h>
#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)

#endif

// Other common inclusion

#include "Types.h"
