#pragma once

#include "Batang/Singleton.h"
#include "Batang/Thread.h"

namespace Maragi
{
    class MainController final : public Batang::Thread<MainController>, public Batang::Singleton<MainController>
    {
    private:
        boostpo::variables_map cmdLine_;
        bool help_;

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
