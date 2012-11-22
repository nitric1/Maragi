// $Id: Objects.h 80 2012-11-20 09:10:33Z wdlee91 $

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
				static Point invalid;

				T x, y;

				Point()
					: x(), y()
				{}

				template<typename Other>
				Point(const Point<Other> &that)
					: x(static_cast<T>(that.x)), y(static_cast<T>(that.y))
				{}

				Point(T ix, T iy)
					: x(ix), y(iy)
				{}

				template<typename Other>
				Point &operator =(const Point<Other> &rhs)
				{
					x = static_cast<T>(that.x);
					y = static_cast<T>(that.y);
				}

				bool operator ==(const Point &rhs) const
				{
					return x == rhs.x && y == rhs.y;
				}

				bool operator !=(const Point &rhs) const
				{
					return !(*this == rhs);
				}

				operator D2D1_POINT_2U() const
				{
					return D2D1::Point2U(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
				}

				operator D2D1_POINT_2F() const
				{
					return D2D1::Point2F(static_cast<float>(x), static_cast<float>(y));
				}
			};

			template<typename T>
			Point<T> Point<T>::invalid(
				std::numeric_limits<T>::min() == T(0) /* if T is unsigned */ ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min(),
				std::numeric_limits<T>::min() == T(0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min()
				);

			typedef Point<int32_t> PointI;
			typedef Point<float> PointF;

			template<typename T>
			struct Size
			{
				static Size invalid;

				T width, height;

				Size()
					: width(), height()
				{}

				template<typename Other>
				Size(const Size<Other> &that)
					: width(static_cast<T>(that.width)), height(static_cast<T>(that.height))
				{}

				Size(T iwidth, T iheight)
					: width(iwidth), height(iheight)
				{}

				template<typename Other>
				Size &operator =(const Size<Other> &rhs)
				{
					width = static_cast<T>(rhs.width);
					height = static_cast<T>(rhs.height);
					return *this;
				}

				bool operator ==(const Size &rhs) const
				{
					return width == rhs.width && height == rhs.height;
				}

				bool operator !=(const Size &rhs) const
				{
					return !(*this == rhs);
				}

				operator D2D1_SIZE_U() const
				{
					return D2D1::SizeU(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
				}

				operator D2D1_SIZE_F() const
				{
					return D2D1::SizeF(static_cast<float>(width), static_cast<float>(height));
				}
			};

			template<typename T>
			Size<T> Size<T>::invalid(
				std::numeric_limits<T>::min() == T(0) /* if T is unsigned */ ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min(),
				std::numeric_limits<T>::min() == T(0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min()
				);

			typedef Size<int32_t> SizeI;
			typedef Size<float> SizeF;

			template<typename T>
			struct Rectangle
			{
				static Rectangle invalid;

				T left, top, right, bottom;

				Rectangle()
					: left(), top(), right(), bottom()
				{}

				template<typename Other>
				Rectangle(const Rectangle<Other> &that)
					: left(static_cast<T>(that.left)), top(static_cast<T>(that.top)), right(static_cast<T>(that.right)), bottom(static_cast<T>(that.bottom))
				{}

				Rectangle(T ileft, T itop, T iright, T ibottom)
					: left(ileft), top(itop), right(iright), bottom(ibottom)
				{}

				Rectangle(const Point<T> &lt, const Point<T> &rb)
					: left(lt.x), top(lt.y), right(rb.x), bottom(rb.y)
				{}

				Rectangle(const Point<T> &lt, const Size<T> &size)
					: left(lt.x), top(lt.y), right(lt.x + size.width), bottom(lt.y + size.height)
				{}

				Rectangle(const Point<T> &center, float width, float height) // like ellipse
					: left(center.x - width / 2.0f), top(center.y - height / 2.0f), right(center.x + width / 2.0f), bottom(center.y + height / 2.0f)
				{}

				Point<T> leftTop() const
				{
					return Point<T>(left, top);
				}

				Point<T> rightBottom() const
				{
					return Point<T>(right, bottom);
				}

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
					return Size<T>(width(), height());
				}

				bool isIn(const Point<T> &pt) const
				{
					return (pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom);
				}

				template<typename Other>
				Rectangle &operator =(const Rectangle<Other> &rhs)
				{
					left = static_cast<T>(rhs.left);
					top = static_cast<T>(rhs.top);
					right = static_cast<T>(rhs.right);
					bottom = static_cast<T>(rhs.bottom);
				}

				bool operator ==(const Rectangle &rhs) const
				{
					return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
				}

				bool operator !=(const Rectangle &rhs) const
				{
					return !(*this == rhs);
				}

				operator D2D1_RECT_U() const
				{
					return D2D1::RectU(static_cast<uint32_t>(left), static_cast<uint32_t>(top), static_cast<uint32_t>(right), static_cast<uint32_t>(bottom));
				}

				operator D2D1_RECT_F() const
				{
					return D2D1::RectF(static_cast<float>(left), static_cast<float>(top), static_cast<float>(right), static_cast<float>(bottom));
				}
			};

			template<typename T>
			Rectangle<T> Rectangle<T>::invalid(
				std::numeric_limits<T>::min() == T(0) /* if T is unsigned */ ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min(),
				std::numeric_limits<T>::min() == T(0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min(),
				std::numeric_limits<T>::min() == T(0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min(),
				std::numeric_limits<T>::min() == T(0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min()
				);

			typedef Rectangle<int32_t> RectangleI;
			typedef Rectangle<float> RectangleF;

			class ColorF : public D2D1::ColorF
			{
			public:
				ColorF(uint32_t rgb, float a = 1.0f)
					: D2D1::ColorF(rgb, a)
				{}

				ColorF(Enum knownColor, float a = 1.0f)
					: D2D1::ColorF(knownColor, a)
				{}

				ColorF(float r, float g, float b, float a = 1.0f)
					: D2D1::ColorF(r, g, b, a)
				{}

				ColorF(COLORREF bgr, float a = 1.0f)
					: D2D1::ColorF(GetRValue(bgr) << 16 | GetGValue(bgr) << 8 | GetBValue(bgr), a)
				{}

			public:
				const ColorF operator ~() const
				{
					return ColorF(1.0f - r, 1.0f - g, 1.0f - b, a);
				}

				ColorF &invert()
				{
					r = 1.0f - r;
					g = 1.0f - g;
					b = 1.0f - b;
					return *this;
				}
			};
		}
	}
}
