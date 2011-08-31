// $Id$

#include "Defaults.h"

#include "MainController.h"

namespace Maragi
{
	MainController::MainController()
	{
		registerEvents();
	}
	
	MainController::~MainController()
	{
	}

	void MainController::registerEvents()
	{
	}

	bool MainController::run(HINSTANCE instance, const std::wstring &commandLine, int showCommand)
	{
		__try
		{
			return runImpl(instance, commandLine, showCommand);
		}
		__except(filterOSException(GetExceptionCode(), GetExceptionInformation()))
		{
			return false;
		}
	}

	bool MainController::runImpl(HINSTANCE instance, const std::wstring &commandLine, int showCommand)
	{
		try
		{
			appInst = instance;
			return true;
		}
		catch(std::exception &e)
		{
		}
	}

	int MainController::filterOSException(unsigned icode, EXCEPTION_POINTERS *iep)
	{
	}
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, wchar_t *commandLine, int showCommand)
{
	return Maragi::MainController::instance().run(instance, commandLine, showCommand);
}
