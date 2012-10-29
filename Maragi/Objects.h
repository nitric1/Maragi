// $Id$

#pragma once

namespace Maragi
{
	namespace UI
	{
		namespace Objects
		{
			template<typename T>
			struct Point
			{
				T x, y;

				Point()
					: x(), y()
				{}

				Point(const Point<T> &that)
					: x(that.x), y(that.y)
				{}

				Point(T ix, T iy)
					: x(ix), y(iy)
				{}
			};

			typedef Point<int32_t> PointI;
			typedef Point<float> PointF;

			template<typename T>
			struct Size
			{
				T width, height;

				Size()
					: width(), height()
				{}

				Size(const Size<T> &that)
					: width(that.width), height(that.height)
				{}

				Size(T iwidth, T iheight)
					: width(iwidth), height(iheight)
				{}
			};

			typedef Size<int32_t> SizeI;
			typedef Size<float> SizeF;

			template<typename T>
			struct Rectangle
			{
				T left, top, right, bottom;

				Rectangle()
					: left(), top(), right(), bottom()
				{}

				Rectangle(const Rectangle<T> &that)
					: left(that.left), top(that.top), right(that.right), bottom(that.bottom)
				{}

				Rectangle(T ileft, T itop, T iright, T ibottom)
					: left(ileft), top(itop), right(iright), bottom(ibottom)
				{}

				Rectangle(const Point<T> &lt, const Point<T> &rb)
					: left(lt.x), top(lt.y), right(rb.x), bottom(rb.y)
				{}

				Rectangle(const Point<T> &center, float width, float height) // like ellipse
					: left(center.x - width / 2.0f), top(center.y - height / 2.0f), right(center.x + width / 2.0f), bottom(center.y + height / 2.0f)
				{}

				T width() const
				{
					return right - left;
				}

				T height() const
				{
					return bottom - top;
				}

				Size<T> size() const
				{
					Size<T> sz = { width(), height() };
					return sz;
				}
			};

			typedef Rectangle<int32_t> RectangleI;
			typedef Rectangle<float> RectangleF;
		}
	}
}
