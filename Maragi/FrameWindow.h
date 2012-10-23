// $Id$

#pragma once

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class FrameWindow : public Shell
		{
		private:

		public:
			FrameWindow();
			explicit FrameWindow(Shell *);

		protected:
			virtual ~FrameWindow();
		};
	}
}
