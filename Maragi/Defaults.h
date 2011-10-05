// $Id$

#pragma once

#define NOMINMAX

// Dictionary order
// Standard C/C++ Library Inclusion

#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::max; using std::min;

// External Library Inclusion

#include <curl/curl.h>
#include <json/json.h>
#include <openssl/sha.h>

// Windows API Inclusion; <windows.h> should be first.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <commctrl.h>
#include <commdlg.h>
#include <dlgs.h>
#include <gdiplus.h>
#include <wincrypt.h>
#include <windowsx.h>

// Other Common Inclusion

#include "resource.h"
#include "Types.h"
