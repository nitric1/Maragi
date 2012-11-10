// $Id$

#include "Common.h"

#include "Controls.h"
#include "MainController.h"
#include "Tokens.h"
#include "TwitterClient.h"
#include "Singleton.h"
#include "Window.h"

#pragma warning(push)
#pragma warning(disable:4702)

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

	bool MainController::run(const std::wstring &commandLine, int showCommand)
	{
		__try
		{
			return runImpl(commandLine, showCommand);
		}
		__except(filterOSException(GetExceptionCode(), GetExceptionInformation()))
		{
			return false;
		}
	}

	bool MainController::runImpl(const std::wstring &commandLine, int showCommand)
	{
		try
		{
			if(!checkPrerequisites())
				return false;

			parseCommandLine(commandLine);

			// TwitterClient tc;
			// tc.authorize();

			// mainWin->setShowStatus(showCommand);
			// return mainWin->show();

			UI::ShellPtr<UI::FrameWindow> frm = UI::FrameWindow::create(
				nullptr,
				L"Maragi",
				UI::Resources::Icon::fromSharedResource(IDI_APPLICATION, UI::Objects::SizeI(32, 32)),
				UI::Resources::Icon::fromSharedResource(IDI_APPLICATION, UI::Objects::SizeI(16, 16)),
				UI::Objects::ColorF::Black,
				UI::Objects::SizeI(640, 480),
				UI::Objects::PointI::invalid,
				UI::Objects::SizeI(400, 300),
				UI::Objects::SizeI::invalid
				);
			//UI::ControlPtr<UI::GridLayout> layout = UI::GridLayout::create(1, 2);
			//frm->client->slot.get().attach(layout);
			UI::ControlPtr<UI::Button> button = UI::Button::create(L"Button Text");
			frm->client->slot->attach(button);
			//layout->slot[0][1].attach(button);
			//button.onClick += delegate(this, &onButtonClick);
			return frm->show(showCommand);
		}
		catch(std::exception &e)
		{
			std::string str = "Unexpected exception occured: ";
			str += e.what();
			str += "\nThe program will be terminated.";
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
	return Maragi::MainController::instance().run(commandLine, showCommand) ? 0 : 1;
}

#pragma warning(pop)
