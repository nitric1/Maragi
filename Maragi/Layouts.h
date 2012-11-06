// $Id$

#pragma once

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Layout : public Control
		{
		protected:
			Layout(const ControlWeakPtr<> &, const ControlID &);
		};

		class ShellLayout : public Layout
		{
		private:
			ShellWeakPtr<> shell_;

		protected:
			ShellLayout(const ShellWeakPtr<> &, const ControlID &);
			virtual ~ShellLayout();

		public:
			static ControlPtr<ShellLayout> create(
				const ShellWeakPtr<> &
				);

		public:
			virtual void draw(Context &);

		public:
			Property::R<ShellLayout, ShellWeakPtr<>> shell;

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
