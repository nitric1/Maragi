// $Id: ComUtility.cpp 61 2012-11-06 09:08:38Z wdlee91 $

#pragma once

#include "Common.h"

#include "ComUtility.h"

namespace Maragi
{
	namespace UI
	{
		ComInitializer::ComInitializer()
		{
			HRESULT hr;
			hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if(FAILED(hr))
				throw(ComException("CoInitializeEx failed."));
		}

		ComInitializer::~ComInitializer()
		{
			CoUninitialize();
		}
	}
}
