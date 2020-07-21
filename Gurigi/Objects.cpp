#include "Common.h"

#include "../Batang/Utility.h"

#include "Drawing.h"
#include "Global.h"
#include "Objects.h"
#include "Window.h"

namespace Gurigi
{
    namespace Objects
    {
        namespace
        {
            uint32_t getDpi(const Shell &shell)
            {
                return GetDpiForWindow(shell.hwnd());
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

        PointI convertPoint(const Shell &shell, const PointF &pt)
        {
            auto dpi = getDpi(shell);
            return PointI(toRawPixel(pt.x, dpi), toRawPixel(pt.y, dpi));
        }

        PointF convertPoint(const Shell &shell, const PointI &pt)
        {
            auto dpi = getDpi(shell);
            return PointF(fromRawPixel(pt.x, dpi), fromRawPixel(pt.y, dpi));
        }

        SizeI convertSize(const Shell &shell, const SizeF &size)
        {
            auto dpi = getDpi(shell);
            return SizeI(toRawPixel(size.width, dpi), toRawPixel(size.height, dpi));
        }

        SizeF convertSize(const Shell &shell, const SizeI &size)
        {
            auto dpi = getDpi(shell);
            return SizeF(fromRawPixel(size.width, dpi), fromRawPixel(size.height, dpi));
        }

        RectangleI convertRectangle(const Shell &shell, const RectangleF &rect)
        {
            auto dpi = getDpi(shell);
            return RectangleI(
                toRawPixel(rect.left, dpi), toRawPixel(rect.top, dpi),
                toRawPixel(rect.right, dpi), toRawPixel(rect.bottom, dpi));
        }

        RectangleF convertRectangle(const Shell &shell, const RectangleI &rect)
        {
            auto dpi = getDpi(shell);
            return RectangleF(
                fromRawPixel(rect.left, dpi), fromRawPixel(rect.top, dpi),
                fromRawPixel(rect.right, dpi), fromRawPixel(rect.bottom, dpi));
        }
    }
}
