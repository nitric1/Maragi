// $Id$

#include "Common.h"

#include "MainController.h"

#include "Tokens.h"
#include "TwitterClient.h"

#pragma warning(push)
#pragma warning(disable:4702)

namespace Maragi
{
	MainController::MainController()
	{
		registerEvents();
//		mainWin = &MainWindow::instance();
	}
	
	MainController::~MainController()
	{
	}

	HINSTANCE MainController::getInstance() const
	{
		return appInst;
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

			if(!checkPrerequisites())
				return false;

			parseCommandLine(commandLine);

			TwitterClient tc;
			tc.authorize();

			return true;
		}
		catch(std::exception &e)
		{
			std::string str = std::string("Unexpected exception occured: ")
				+ e.what()
				+ std::string("\nThe program will be terminated.");
			MessageBoxA(nullptr, str.c_str(), "Error occured", MB_ICONSTOP | MB_OK);
			return false;
		}
	}

	void MainController::registerEvents()
	{
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

	bool MainController::checkPrerequisites()
	{
		if(strcmp(AppTokens::CONSUMER_KEY, "") == 0 || strcmp(AppTokens::CONSUMER_SECRET, "") == 0)
		{
			throw(std::logic_error("Tokens for authorization are not filled. Check Tokens.h.in."));
			return false;
		}

		return true;
	}

	void MainController::parseCommandLine(const std::wstring &commandLine)
	{
		cmdLine.addKey(L"help", false);
		cmdLine.addAbbr(L"h", L"help");
		cmdLine.addAbbr(L"?", L"help");

		cmdLine.parse(commandLine);

		if(!cmdLine.isArgEmpty(L"help"))
		{
			// TODO: Maragi.exe --help
		}
	}
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, wchar_t *commandLine, int showCommand)
{
	return Maragi::MainController::instance().run(instance, commandLine, showCommand) ? 0 : 1;
}

#pragma warning(pop)
