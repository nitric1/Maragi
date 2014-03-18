#pragma once

#include "Batang/CommandLineParser.h"
#include "Batang/Singleton.h"
#include "Batang/Thread.h"

namespace Maragi
{
    class MainController : public Batang::Thread<MainController>, public Batang::Singleton<MainController>
    {
    private:
        Batang::CommandLineParser cmdLine_;

    private:
        MainController();
        ~MainController();

    private:
        bool run(const std::wstring &, int);
        bool runImpl(const std::wstring &, int);

    private:
        void registerEvents();
        int filterOSException(unsigned, EXCEPTION_POINTERS *);
        bool showOSException(unsigned, EXCEPTION_POINTERS *);
        bool checkPrerequisites();
        void parseCommandLine(const std::wstring &);

        friend class Batang::Thread<MainController>;
        friend class Batang::Singleton<MainController>;
    };
}
