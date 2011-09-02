// $Id$

#pragma once

#include "Singleton.h"
#include "MainWindow.h"

namespace Maragi
{
	class MainController : public Singleton<MainController>
	{
	private:
		unsigned exceptionCode;
		EXCEPTION_POINTERS exceptionPointers;
		HINSTANCE appInst;
		MainWindow *mainWin;

	public:
		MainController();
		~MainController();

	public:
		void registerEvents();
		bool run(HINSTANCE, const std::wstring &, int);
		bool runImpl(HINSTANCE, const std::wstring &, int);
		int filterOSException(unsigned, EXCEPTION_POINTERS *);
		bool showOSException();
	};
}
