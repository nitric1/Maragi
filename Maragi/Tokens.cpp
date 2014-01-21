#include "Common.h"

#include "Tokens.h"

namespace
{
#ifndef _MSC_VER // VC 2013 does not support constexpr string literal
    enum
    {
        ConsumerKeyFirst = MARAGI_CONSUMER_KEY[0],
        ConsumerSecretFirst = MARAGI_CONSUMER_SECRET[0],
    };

    static_assert(ConsumerKeyFirst != '\0' && ConsumerSecretFirst != '\0',
        "Tokens for authorization are not filled. Check Tokens.h.in and Tokens.txt.");
#endif
}

namespace Maragi
{
    namespace AppTokens
    {
        const char * const CONSUMER_KEY = MARAGI_CONSUMER_KEY;
        const char * const CONSUMER_SECRET = MARAGI_CONSUMER_SECRET;
    }
}
