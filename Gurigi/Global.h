#pragma once

#include "Objects.h"

namespace Gurigi
{
    Objects::PointI getCursorPos();
    Objects::PointF translatePointIn(const Objects::PointF &, const Objects::RectangleF &);
    // Objects::PointF translatePointOut();
}
