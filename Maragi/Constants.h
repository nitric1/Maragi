﻿#pragma once

#define MARAGI_TITLE L"Maragi"
#define MARAGI_VERSION L"0.0.1"

namespace Maragi
{
    namespace Constants
    {
        extern const wchar_t * const TITLE, * const VERSION, * const FULL_NAME, * const USER_AGENT;
    }

    namespace Paths
    {
        extern const char * const PROTOCOL, * const HOST;
        extern const char * const REQUEST_TOKEN, * const AUTHORIZE, * const ACCESS_TOKEN;
    }
}
