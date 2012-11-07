// $Id$

#pragma once

#include "ComUtility.h"
#include "Drawing.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Layout : public Control
		{
		protected:
			Layout(Slot *, const ControlID &);
		};

		class ShellLayout : public Layout
		{
		private:
			ShellWeakPtr<> shell_;
			ControlWeakPtr<> child_;

			ComPtr<ID2D1SolidColorBrush> brush;

		protected:
			ShellLayout(const ShellWeakPtr<> &, const ControlID &);
			virtual ~ShellLayout();

		public:
			static ControlPtr<ShellLayout> create(
				const ShellWeakPtr<> &
				);

		public:
			virtual void createDrawingResources(Drawing::Context &);
			virtual void discardDrawingResources(Drawing::Context &);
			virtual void draw(Drawing::Context &);

		public:
			Property::R<ShellLayout, ShellWeakPtr<>> shell;
			Property::R<ShellLayout, ControlWeakPtr<>> child;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class GridLayout : public Layout
		{
		public:
			class RowProxy
			{
			};

			class CellProxy : public Control
			{
			};
		};
	}
}
