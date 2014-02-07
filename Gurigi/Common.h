#pragma once

#include "Batang/Common.h"

// All inclusion listing is dictionary order; Ordering is case insensitive.
// Standard C/C++ library & boost library inclusion

#include <boost/date_time/posix_time/posix_time_types.hpp>

#pragma warning(push)
#pragma warning(disable: 4819) // The file contains a character that cannot be represented in the current code page
#include <boost/format.hpp>
#pragma warning(pop)

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
