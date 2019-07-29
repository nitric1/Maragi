#pragma once

#include "../External/include/ERDelegate.h"

namespace Batang
{
    template<typename Func>
    inline ERDelegateWrapper<typename ERDelegateFunctionTraits<Func>::FunctionType> delegate(Func &&fn)
    {
        return std::shared_ptr<ERDelegate<typename ERDelegateFunctionTraits<Func>::FunctionType>>(
            new ERDelegateImpl<typename ERDelegateFunctionTraits<Func>::FunctionType>(std::forward<Func>(fn)));
    }

    template<typename Class, typename Func>
    inline ERDelegateWrapper<typename ERDelegateFunctionTraits<Func>::FunctionType> delegate(Class *p, Func &&fn)
    {
        return std::shared_ptr<ERDelegate<typename ERDelegateFunctionTraits<Func>::FunctionType>>(
            new ERDelegateImpl<typename ERDelegateFunctionTraits<Func>::FunctionType>(std::forward<Func>(fn), p));
    }
}
