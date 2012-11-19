// $Id$

#pragma once

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Label : public Control
		{
		public:
			enum
			{
				LEFT = 0x00000000,
				CENTER = 0x00000001,
				RIGHT = 0x00000002,
				TOP = 0x00000000,
				VCENTER = 0x00000004,
				BOTTOM = 0x00000008
			};

			enum
			{
				HORZ_MASK = 0x00000003,
				VERT_MASK = 0x0000000C
			};

		private:
			std::wstring text_;
			Objects::ColorF color_;
			uint32_t align_;

			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<IDWriteTextFormat> format;
			ComPtr<IDWriteRenderingParams> renderParams;

		protected:
			Label(const ControlID &);
			virtual ~Label();

		public:
			static ControlPtr<Label> create(
				const std::wstring &,
				const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
				uint32_t = LEFT | VCENTER
				);

		public:
			virtual void createDrawingResources(Drawing::Context &);
			virtual void discardDrawingResources(Drawing::Context &);
			virtual void draw(Drawing::Context &);
			virtual Objects::SizeF computeSize();

		public:
			Property::RW<Label, std::wstring> text;
			Property::RW<Label, Objects::ColorF> color;
			Property::RW<Label, uint32_t> align;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class Button : public Control
		{
		private:
			std::wstring text_;

			ComPtr<ID2D1SolidColorBrush> brushUp, brushDown;
			bool clicked;
			Objects::PointF pt;

		protected:
			Button(const ControlID &);
			virtual ~Button();

		public:
			static ControlPtr<Button> create(
				const std::wstring &
				);

		public:
			virtual void createDrawingResources(Drawing::Context &);
			virtual void discardDrawingResources(Drawing::Context &);
			virtual void draw(Drawing::Context &);
			virtual Objects::SizeF computeSize();

		private:
			void onMouseButtonDownImpl(const ControlEventArg &);
			void onMouseButtonUpImpl(const ControlEventArg &);

		public:
			ControlEvent onClick;

		public:
			Property::RW<Button, std::wstring> text;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};
	}
}
