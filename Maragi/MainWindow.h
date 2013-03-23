#pragma once

#include "FrameWindow.h"
#include "Singleton.h"

namespace Maragi
{
    class MainWindow : public UI::FrameWindow, public Singleton<MainWindow>
    {
    private:
        MainWindow();
        virtual ~MainWindow();

    public:

        friend class Singleton<MainWindow>;
    };
}
