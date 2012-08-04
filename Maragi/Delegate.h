// $Id$

#pragma once

#include "ERDelegate.h"

namespace Maragi
{
	template<typename FunctionType, typename Func>
	inline std::shared_ptr<ERDelegate<FunctionType>> delegate(Func fn)
	{
		return std::shared_ptr<ERDelegate<FunctionType>>(new ERDelegateImpl<FunctionType>(fn));
	}

	template<typename FunctionType, typename Class, typename Func>
	inline std::shared_ptr<ERDelegate<FunctionType>> delegate(Class *p, Func fn)
	{
		return std::shared_ptr<ERDelegate<FunctionType>>(new ERDelegateImpl<FunctionType>(fn, p));
	}
}
