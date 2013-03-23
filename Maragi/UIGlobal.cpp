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

        Objects::PointF translatePointIn(const Objects::PointF &pt, const Objects::RectangleF &rect)
        {
            return Objects::PointF(pt.x - rect.left, pt.y - rect.top);
        }
    }
}
