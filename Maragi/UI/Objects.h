// $Id$

#pragma once

namespace Maragi
{
	namespace UI
	{
		namespace Objects
		{
			struct Rectangle
			{
				int32_t left, top, right, bottom;

				Rectangle()
					: left(0), top(0), right(0), bottom(0)
				{}

				Rectangle(const Rectangle &obj)
					: left(obj.left), top(obj.top), right(obj.right), bottom(obj.bottom)
				{}

				Rectangle(const RECT &rc)
					: left(rc.left), top(rc.top), right(rc.right), bottom(rc.bottom)
				{}

				Rectangle(int32_t ileft, int32_t itop, int32_t iright, int32_t ibottom)
					: left(ileft), top(itop), right(iright), bottom(ibottom)
				{}

				Rectangle &operator =(const Rectangle &rhs)
				{
					left = rhs.left;
					top = rhs.top;
					right = rhs.right;
					bottom = rhs.bottom;
					return *this;
				}

				Rectangle &operator =(const RECT &rc)
				{
					left = rc.left;
					top = rc.top;
					right = rc.right;
					bottom = rc.bottom;
					return *this;
				}

				operator RECT()
				{
					RECT rc = {left, top, right, bottom};
					return rc;
				}

				int32_t width()
				{
					return right - left;
				}

				int32_t height()
				{
					return bottom - top;
				}
			};
		}
	}
}
