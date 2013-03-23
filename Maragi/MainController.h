#pragma once

#include "CommandLineParser.h"
#include "Singleton.h"
#include "MainWindow.h"

namespace Maragi
{
    class MainController : public Singleton<MainController>
    {
    private:
        unsigned exceptionCode;
        EXCEPTION_POINTERS exceptionPointers;
        MainWindow *mainWin;
        CommandLineParser cmdLine;

    private:
        MainController();
        ~MainController();

    public:
        bool run(const std::wstring &, int);
        bool runImpl(const std::wstring &, int);

    private:
        void registerEvents();
        int filterOSException(unsigned, EXCEPTION_POINTERS *);
        bool showOSException();
        bool checkPrerequisites();
        void parseCommandLine(const std::wstring &);

    private:
        void onButtonClick(const UI::ControlEventArg &);

        friend class Singleton<MainController>;
    };
}
