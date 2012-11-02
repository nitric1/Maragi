// $Id$

#pragma once

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Layout : public Control
		{
		};

		class FullLayout : public Layout
		{
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
