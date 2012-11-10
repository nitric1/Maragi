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
			Layout(const ControlID &);

		public:
			virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) = 0;
		};

		class ShellLayout : public Layout
		{
		private:
			ShellWeakPtr<> shell_;
			Slot slot_;

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
			virtual Objects::SizeF computeSize();
			virtual ControlWeakPtr<> findByPoint(const Objects::PointF &);

		protected:
			virtual void onResizeInternal(const Objects::RectangleF &);

		public:
			Property::R<ShellLayout, ShellWeakPtr<>> shell;
			Property::R<ShellLayout, Slot *> slot;

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
