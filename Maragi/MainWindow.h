#pragma once

#include "Batang/Singleton.h"

#include "Gurigi/FrameWindow.h"

namespace Maragi
{
    class MainWindow : public Gurigi::FrameWindow, public Batang::Singleton<MainWindow>
    {
    private:
        MainWindow();
        virtual ~MainWindow();

    public:

        friend class Batang::Singleton<MainWindow>;
    };
}
