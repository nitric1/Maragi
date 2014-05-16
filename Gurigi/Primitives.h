#pragma once

#include "Batang/Wrapper.h"

namespace Gurigi
{
    struct ControlIdTag {};
    typedef Batang::ValueWrapper::Wrapper<size_t, ControlIdTag, 0> ControlId;

    class UIException : public std::runtime_error
    {
    public:
        UIException()
            : std::runtime_error("UIException")
        {}

        explicit UIException(const std::string &str)
            : std::runtime_error(str)
        {}

        explicit UIException(const char *str)
            : std::runtime_error(str)
        {}
    };
}
