// $Id$

#include "Common.h"

#include "UIGlobal.h"

namespace Maragi
{
	namespace UI
	{
		Objects::PointI getCursorPos()
		{
			POINT pt;
			GetCursorPos(&pt);
			return Objects::PointI(pt.x, pt.y);
		}
	}
}
