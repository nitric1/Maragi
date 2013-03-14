// $Id: MainWindow.h 48 2012-10-23 15:21:09Z wdlee91 $

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
