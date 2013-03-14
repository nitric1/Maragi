// $Id: UIGlobal.h 73 2012-11-13 09:11:48Z wdlee91 $

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
