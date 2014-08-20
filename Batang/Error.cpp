#include "Common.h"

#include "Error.h"

namespace Batang
{
    namespace Detail
    {
        void Assert(bool cond, const char *condStr, std::string file, uint32_t line, std::string func)
        {
            Assert(cond, condStr, file, line, func, condStr);
        }

        void Assert(bool cond, const char *, std::string file, uint32_t line, std::string func, std::string message)
        {
            if(!cond)
            {
                throw(AssertError(std::move(message), std::move(file), line, std::move(func)));
            }
        }
    }

    AssertError::AssertError(std::string what, std::string file, uint32_t line, std::string func)
        : logic_error(what)
        , file_(std::move(file))
        , line_(line)
        , func_(func)
    {
    }
}
