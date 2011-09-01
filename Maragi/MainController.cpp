// $Id$

#include "Defaults.h"

#include "MainController.h"

namespace Maragi
{
	MainController::MainController()
	{
		registerEvents();
		mainWin = &MainWindow::instance();
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
			std::string str = std::string("Unexpected exception occured: ") + e.what() + std::string("\nThe program will be terminated.");
			MessageBoxA(nullptr, str.c_str(), "Error occured", MB_ICONSTOP | MB_OK);
			return false;
		}
	}

	int MainController::filterOSException(unsigned icode, EXCEPTION_POINTERS *iep)
	{
		exceptionCode = icode;
		memcpy(&exceptionPointers, iep, sizeof(exceptionPointers));

		return showOSException() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
	}

	bool MainController::showOSException()
	{
		// TODO: Implementaion exception showing
		return false;
	}
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, wchar_t *commandLine, int showCommand)
{
	return Maragi::MainController::instance().run(instance, commandLine, showCommand) ? 0 : 1;
}
