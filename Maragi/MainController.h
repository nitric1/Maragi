// $Id$

#pragma once

#include "Singleton.h"

namespace Maragi
{
	class MainController : public Singleton<MainController>
	{
	private:
		HINSTANCE appInst;

	public:
		MainController();
		~MainController();

	public:
		void registerEvents();
		bool run(HINSTANCE, const std::wstring &, int);
		bool runImpl(HINSTANCE, const std::wstring &, int);
		int filterOSException(unsigned, EXCEPTION_POINTERS *)
	};
}
