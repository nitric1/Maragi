// $Id$

#pragma once

#include "Drawing.h"
#include "Layouts.h"
#include "Resources.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class FrameWindow : public Shell
		{
		public:
			static const uint32_t windowStyle;
			static const uint32_t windowStyleEx;

		private:
			ControlPtr<ShellLayout> client_; // FrameWindow handles only one child.
			Resources::ResourcePtr<Resources::Icon> iconLarge_, iconSmall_;
			Objects::ColorF bgColor_;
			Objects::SizeI minClientSize_, maxClientSize_;
			Drawing::Context context_;

			std::wstring className;
			std::wstring initTitle;
			Objects::PointI initPosition;
			Objects::SizeI initClientSize;
			bool inDestroy;

		protected:
			FrameWindow();
			explicit FrameWindow(const ShellWeakPtr<> &);
			virtual ~FrameWindow();

		public:
			static ShellPtr<FrameWindow> create(
				const ShellWeakPtr<> &,
				const std::wstring &,
				const Resources::ResourcePtr<Resources::Icon> &,
				const Resources::ResourcePtr<Resources::Icon> &,
				const Objects::ColorF & = Objects::ColorF(Objects::ColorF::White),
				const Objects::SizeI & = Objects::SizeI::invalid,
				const Objects::PointI & = Objects::PointI::invalid,
				const Objects::SizeI & = Objects::SizeI::invalid,
				const Objects::SizeI & = Objects::SizeI::invalid
				); // TODO: menu

		public:
			virtual bool show();
			virtual bool show(int32_t);
			virtual Objects::SizeI adjustWindowSize(const Objects::SizeI &);
			virtual Objects::PointF screenToClient(const Objects::PointI &) const;
			virtual Objects::PointI clientToScreen(const Objects::PointF &) const;
			virtual ControlWeakPtr<> hoveredControl(const Objects::PointF &) const;
			virtual void redraw();

		public:
			Property::R<FrameWindow, ControlPtr<ShellLayout>> client;
			Property::RW<FrameWindow, Resources::ResourcePtr<Resources::Icon>> iconLarge, iconSmall;
			Property::RW<FrameWindow, Objects::ColorF> bgColor;
			Property::RW<FrameWindow, Objects::SizeI> clientSize;
			Property::RW<FrameWindow, Objects::SizeI> windowSize;
			Property::RW<FrameWindow, Objects::PointI> position;
			Property::RW<FrameWindow, Objects::SizeI> minClientSize, maxClientSize;

		private:
			virtual longptr_t procMessage(HWND, unsigned, uintptr_t, longptr_t);

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};
	}
}
