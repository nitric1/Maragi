// $Id: MainController.cpp 83 2012-11-21 06:12:41Z wdlee91 $

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
				UI::Objects::SizeF(640.0f, 480.0f),
				UI::Objects::PointI::invalid,
				UI::Objects::SizeF(400.0f, 300.0f),
				UI::Objects::SizeF::invalid
				);

			using UI::GridSize;
			GridSize rows[] = { GridSize(20.0f), GridSize(1) }, cols[] = { GridSize(120.0f), GridSize(1) };
			UI::ControlPtr<UI::GridLayout<2, 2>> layout = UI::GridLayout<2, 2>::create(
				std::vector<GridSize>(std::begin(rows), std::end(rows)),
				std::vector<GridSize>(std::begin(cols), std::end(cols))
				);
			frm->client()->slot()->attach(layout);

			UI::ControlPtr<UI::Label> label;
			UI::ControlPtr<UI::Button> button;
			UI::ControlPtr<UI::Edit> edit;

			label = UI::Label::create(L"Label Text", UI::Objects::ColorF(UI::Objects::ColorF::White));
			layout->slot(0, 0)->attach(label);

			button = UI::Button::create(L"Button Text");
			button->onClick += delegate([&edit](const UI::ControlEventArg &)
			{
				if(edit->text().empty())
				{
					edit->text(L"한글 시험 World Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce ac lectus lacus. Integer ac nisi a augue fringilla porttitor. Suspendisse id lorem mauris. Nulla vestibulum, enim quis malesuada euismod, ipsum urna dictum ligula, sed placerat ipsum diam vitae ante. Cras condimentum blandit sollicitudin. Aenean vitae nibh nisl, ac iaculis odio. Nullam ut urna risus. Vivamus nec ipsum dolor. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Phasellus nulla neque, tempor a pharetra a, bibendum vitae lacus. 한글 워드랩도 테스트하고 싶습니다. 한글 워드랩은 어떻게 생겨먹었습니까?");
					edit->selection(0, 587, true);
					//edit->text(L"الديباجة한글 한글 English");
					//edit->selection(2, 13, true);
				}
				else
					edit->text(L"");
			});
			layout->slot(0, 1)->attach(button);

			edit = UI::Edit::create(L"Placeholder Now");
			layout->slot(1, 1)->attach(edit);

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

	void MainController::onButtonClick(const UI::ControlEventArg &arg)
	{
		MessageBoxW(nullptr, L"Button Clicked", L"Maragi", MB_OK);
	}
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE, wchar_t *commandLine, int showCommand)
{
	return Maragi::MainController::instance().run(commandLine, showCommand) ? 0 : 1;
}

#pragma warning(pop)
