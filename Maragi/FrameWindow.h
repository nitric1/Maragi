// $Id$

#pragma once

#include "Layouts.h"
#include "Resources.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class FrameWindow : public Shell
		{
		private:
			ControlPtr<FullLayout> client_; // FrameWindow handles only one child.

		public:
			FrameWindow();
			explicit FrameWindow(Shell *);

		protected:
			virtual ~FrameWindow();

		public:
			static ShellPtr<FrameWindow> create(const std::wstring &, const Resources::Icon &, const Objects::SizeI &); // TODO: menu

		public:
			Property::R<FrameWindow, ControlPtr<FullLayout>> client;

		private:
			longptr_t procMessage(HWND, uint32_t, uintptr_t, longptr_t);
		};
	}
}
