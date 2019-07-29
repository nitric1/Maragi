#include "Common.h"

#include "../Batang/Utility.h"

#include "Drawing.h"
#include "Global.h"
#include "Objects.h"

namespace Gurigi
{
    namespace Objects
    {
        namespace
        {
            std::pair<float, float> getDpi()
            {
                float dpiX, dpiY;
                Drawing::D2DFactory::instance().getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
                return std::make_pair(dpiX, dpiY);
            }

            int32_t toRawPixel(float val, float dpi)
            {
                return static_cast<int32_t>(Batang::round(val * dpi / 96.0f));
            }

            float fromRawPixel(int32_t val, float dpi)
            {
                return val * 96.0f / dpi;
            }
        }

        PointI convertPoint(const PointF &pt)
        {
            auto dpi = getDpi();
            return PointI(toRawPixel(pt.x, dpi.first), toRawPixel(pt.y, dpi.second));
        }

        PointF convertPoint(const PointI &pt)
        {
            auto dpi = getDpi();
            return PointF(fromRawPixel(pt.x, dpi.first), fromRawPixel(pt.y, dpi.second));
        }

        SizeI convertSize(const SizeF &size)
        {
            auto dpi = getDpi();
            return SizeI(toRawPixel(size.width, dpi.first), toRawPixel(size.height, dpi.second));
        }

        SizeF convertSize(const SizeI &size)
        {
            auto dpi = getDpi();
            return SizeF(fromRawPixel(size.width, dpi.first), fromRawPixel(size.height, dpi.second));
        }

        RectangleI convertRectangle(const RectangleF &rect)
        {
            auto dpi = getDpi();
            return RectangleI(
                toRawPixel(rect.left, dpi.first), toRawPixel(rect.top, dpi.second),
                toRawPixel(rect.right, dpi.first), toRawPixel(rect.bottom, dpi.second));
        }

        RectangleF convertRectangle(const RectangleI &rect)
        {
            auto dpi = getDpi();
            return RectangleF(
                fromRawPixel(rect.left, dpi.first), fromRawPixel(rect.top, dpi.second),
                fromRawPixel(rect.right, dpi.first), fromRawPixel(rect.bottom, dpi.second));
        }
    }
}
