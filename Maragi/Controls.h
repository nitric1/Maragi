// $Id$

#pragma once

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Label : public Control
		{
		private:
			std::wstring text_;

		protected:
			Label(const ControlID &);
			virtual ~Label();

		public:
			static ControlPtr<Label> create(
				const std::wstring &
				);

		public:
			virtual void draw(Drawing::Context &);
			virtual Objects::SizeF computeSize();

		public:
			Property::RW<Label, std::wstring> text;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class Button : public Control
		{
		private:
			std::wstring text_;
			ComPtr<ID2D1SolidColorBrush> brush;

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

		protected:
			virtual void onMouseButtonDownImpl(const ControlEventArg &);
			virtual void onMouseButtonUpImpl(const ControlEventArg &);

		public:
			Property::RW<Button, std::wstring> text;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};
	}
}
