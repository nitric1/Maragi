// $Id$

#pragma once

#include "Singleton.h"
#include "UI/FrameWindow.h"

namespace Maragi
{
	//class MainWindow : public Window<>, public Singleton<MainWindow>
	class MainWindow : public UI::FrameWindow, public Singleton<MainWindow>
	{
	private:
		MainWindow();
		virtual ~MainWindow();

	public:

		friend class Singleton<MainWindow>;
	};
}
