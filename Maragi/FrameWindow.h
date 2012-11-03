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
			Resources::ResourcePtr<Resources::Icon> iconLarge_, iconSmall_;

		protected:
			FrameWindow();
			explicit FrameWindow(const ShellPtr<Shell> &);
			virtual ~FrameWindow();

		public:
			static ShellPtr<FrameWindow> create(
				const ShellPtr<Shell> &,
				const std::wstring &,
				const Resources::ResourcePtr<Resources::Icon> &,
				const Resources::ResourcePtr<Resources::Icon> &,
				const Objects::PointI &,
				const Objects::SizeI &
				); // TODO: menu

		public:
			Property::R<FrameWindow, ControlPtr<FullLayout>> client;
			Property::RW<FrameWindow, Resources::ResourcePtr<Resources::Icon>> iconLarge, iconSmall;
			Property::RW<FrameWindow, Objects::SizeI> clientSize;

		private:
			virtual longptr_t procMessage(HWND, unsigned, uintptr_t, longptr_t);

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};
	}
}
