#pragma once

#ifdef NDEBUG
#define BATANG_ASSERT(...) ((void)0)
#else
#define BATANG_ASSERT(cond, ...) \
    ((void)(!!(cond) || (::Batang::Detail::Assert(cond, #cond, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION, ##__VA_ARGS__), 0)))
#endif

namespace Batang
{
    namespace Detail
    {
        void Assert(bool cond, const char *condStr, std::string file, uint32_t line, std::string func);
        void Assert(bool cond, const char *, std::string file, uint32_t line, std::string func, std::string message);
    }

    class AssertError : public std::logic_error
    {
    private:
        std::string file_;
        uint32_t line_;
        std::string func_;

    private:
        AssertError(std::string what, std::string file, uint32_t line, std::string func);

    public:
        const std::string &File() const { return file_; }
        uint32_t Line() const { return line_; }
        const std::string &Function() const { return func_; }

        friend void Detail::Assert(bool cond, const char *condStr,
            std::string file, uint32_t line, std::string func, std::string message);
    };
}
