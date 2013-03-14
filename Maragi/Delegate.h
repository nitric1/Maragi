// $Id: Delegate.h 79 2012-11-19 20:08:06Z wdlee91 $

#pragma once

#include "ERDelegate.h"

namespace Maragi
{
	template<typename Func>
	inline ERDelegateWrapper<typename ERDelegateFunctionTraits<Func>::FunctionType> delegate(Func fn)
	{
		return std::shared_ptr<ERDelegate<typename ERDelegateFunctionTraits<Func>::FunctionType>>(new ERDelegateImpl<typename ERDelegateFunctionTraits<Func>::FunctionType>(fn));
	}

	template<typename Class, typename Func>
	inline ERDelegateWrapper<typename ERDelegateFunctionTraits<Func>::FunctionType> delegate(Class *p, Func fn)
	{
		return std::shared_ptr<ERDelegate<typename ERDelegateFunctionTraits<Func>::FunctionType>>(new ERDelegateImpl<typename ERDelegateFunctionTraits<Func>::FunctionType>(fn, p));
	}
}
