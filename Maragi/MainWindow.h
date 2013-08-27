#pragma once

#include "FrameWindow.h"
#include "Batang/Singleton.h"

namespace Maragi
{
    class MainWindow : public UI::FrameWindow, public Batang::Singleton<MainWindow>
    {
    private:
        MainWindow();
        virtual ~MainWindow();

    public:

        friend class Batang::Singleton<MainWindow>;
    };
}
