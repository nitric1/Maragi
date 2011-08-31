// $Id$

#pragma once

#include "Singleton.h"
#include "Window.h"

namespace Maragi
{
	class MainWindow : public Window<>, Singleton<MainWindow>
	{
	public:
		MainWindow();
		virtual ~MainWindow();
	};
}
