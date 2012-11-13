// $Id$

#pragma once

#include "Objects.h"

namespace Maragi
{
	namespace UI
	{
		Objects::PointI getCursorPos();
		Objects::PointF translatePointIn(const Objects::PointF &, const Objects::RectangleF &);
		// Objects::PointF translatePointOut();
	}
}
