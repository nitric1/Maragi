#include "Common.h"

#include "Batang/Timer.h"

#include "Gurigi/Controls.h"
#include "Gurigi/Window.h"

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

            Gurigi::ShellPtr<Gurigi::FrameWindow> frm = Gurigi::FrameWindow::create(
                nullptr,
                L"Maragi",
                Gurigi::Resources::Icon::fromSharedResource(IDI_APPLICATION, Gurigi::Objects::SizeI(32, 32)),
                Gurigi::Resources::Icon::fromSharedResource(IDI_APPLICATION, Gurigi::Objects::SizeI(16, 16)),
                Gurigi::Objects::ColorF::Black,
                Gurigi::Objects::SizeF(640.0f, 480.0f),
                Gurigi::Objects::PointI::invalid,
                Gurigi::Objects::SizeF(400.0f, 300.0f),
                Gurigi::Objects::SizeF::invalid
                );

            frm->onTaskProcessable += delegate(this, &MainController::process);

            using Gurigi::GridSize;
            GridSize rows[] = { GridSize(20.0f), GridSize(1) }, cols[] = { GridSize(120.0f), GridSize(1) };
            Gurigi::ControlPtr<Gurigi::GridLayout<2, 2>> layout = Gurigi::GridLayout<2, 2>::create(
                std::vector<GridSize>(std::begin(rows), std::end(rows)),
                std::vector<GridSize>(std::begin(cols), std::end(cols))
                );
            frm->client()->slot()->attach(layout);

            Gurigi::ControlPtr<Gurigi::Label> label;
            Gurigi::ControlPtr<Gurigi::Button> button;
            Gurigi::ControlPtr<Gurigi::Edit> edit;

            label = Gurigi::Label::create(L"Label Text", Gurigi::Objects::ColorF(Gurigi::Objects::ColorF::White));
            layout->slot(0, 0)->attach(label);

            button = Gurigi::Button::create(L"Button Text");
            button->onClick += Batang::delegate([&edit](const Gurigi::ControlEventArg &)
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

            edit = Gurigi::Edit::create(L"Placeholder Now");
            layout->slot(1, 1)->attach(edit);

            /*std::thread t([this]()
            {
                for(int i = 0; i < 5; ++ i)
                {
                    post([]() { MessageBoxW(nullptr, L"Invoked", L"Invoked", MB_OK); });
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            });
            t.detach();*/

            /*Batang::Timer::instance().installPeriodicTimer(shared_from_this(), std::chrono::seconds(10), []()
            {
                MessageBoxW(nullptr, L"10 sec timer", L"Timer", MB_OK);
            });*/

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

    int MainController::filterOSException(unsigned code, EXCEPTION_POINTERS *ep)
    {
        return showOSException(code, ep) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
    }

    bool MainController::showOSException(unsigned code, EXCEPTION_POINTERS *ep)
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
    return Maragi::MainController::instance().runFromThisThread(commandLine, showCommand) ? 0 : 1;
}

#pragma warning(pop)
