// $Id: Controls.h 84 2012-11-21 12:06:31Z wdlee91 $

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
			virtual const std::wstring &text() const;
			virtual void text(const std::wstring &);
			virtual const Objects::ColorF &color() const;
			virtual void color(const Objects::ColorF &);
			virtual uint32_t align() const;
			virtual void align(uint32_t);
		};

		class Button : public Control
		{
		private:
			std::wstring text_;

			ComPtr<ID2D1SolidColorBrush> brushUp, brushDown, brushHovered;
			bool hovered, clicked;
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
			virtual const std::wstring &text() const;
			virtual void text(const std::wstring &);
		};

		class Edit : public Control
		{
		private:
			std::wstring text_, placeholder_;
			size_t selStart_, selEnd_;
			Objects::ColorF colorText_, colorPlaceholder_, colorBackground_;

			ComPtr<ID2D1SolidColorBrush> brushText, brushPlaceholder, brushBackground;
			ComPtr<IDWriteTextFormat> formatText, formatPlaceholder;
			ComPtr<IDWriteRenderingParams> renderParams;

		protected:
			Edit(const ControlID &);
			virtual ~Edit();

		public:
			static ControlPtr<Edit> create(
				const std::wstring & = std::wstring(),
				const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
				const Objects::ColorF & = Objects::ColorF(Objects::ColorF::DarkGray),
				const Objects::ColorF & = Objects::ColorF(Objects::ColorF::White),
				const std::wstring & = std::wstring(),
				size_t = 0,
				size_t = 0
				);

		public:
			virtual void createDrawingResources(Drawing::Context &);
			virtual void discardDrawingResources(Drawing::Context &);
			virtual void draw(Drawing::Context &);
			virtual Objects::SizeF computeSize();

		private:
			void onMouseMoveImpl(const ControlEventArg &);
			void onMouseButtonDownImpl(const ControlEventArg &);
			void onMouseButtonUpImpl(const ControlEventArg &);
			void onCharImpl(const ControlEventArg &);
			void onImeEndCompositionImpl(const ControlEventArg &);
			void onImeCompositionImpl(const ControlEventArg &);

		public:
			// Event<const std::wstring &> onTextChange;
			// Event<const std::pair<size_t, size_t> &> onSelectionChange;

		public:
			virtual const std::wstring &text() const;
			virtual void text(const std::wstring &);
			virtual const std::wstring &placeholder() const;
			virtual void placeholder(const std::wstring &);
			virtual std::pair<size_t, size_t> selection() const;
			virtual void selection(size_t);
			virtual void selection(size_t, size_t);
		};
	}
}
