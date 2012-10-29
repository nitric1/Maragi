// $Id$

#pragma once

#define NOMINMAX

// All inclusion listing is dictionary order; Ordering is case insensitive.
// Standard C/C++ library inclusion

#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::max; using std::min;

// External library inclusion

#include <curl/curl.h>
#include <json/json.h>
#include <openssl/sha.h>

// Windows API inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include <commctrl.h>
#include <commdlg.h>
#include <d2d1.h>
#include <dlgs.h>
#include <dwrite.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <windowsx.h>

// Other common inclusion

#include "resource.h"
#include "Types.h"
