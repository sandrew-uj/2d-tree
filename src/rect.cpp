#include "primitives.h"

Rect::Rect(const Point & left_bottom, const Point & right_top)
    : left_bottom(left_bottom)
    , right_top(right_top)
{
}
double Rect::xmin() const
{
    return left_bottom.x();
}
double Rect::ymin() const
{
    return left_bottom.y();
}
double Rect::xmax() const
{
    return right_top.x();
}
double Rect::ymax() const
{
    return right_top.y();
}
double Rect::distance(const Point & p) const
{
    double left, top, right, bottom, result;
    left = p.x() < xmin() ? xmin() - p.x() : DBL_MAX;
    top = p.y() > ymax() ? p.y() - ymax() : DBL_MAX;
    right = p.x() > xmax() ? p.x() - xmax() : DBL_MAX;
    bottom = p.y() < ymin() ? ymin() - p.y() : DBL_MAX;
    result = std::min(std::min(left, top), std::min(right, bottom));
    return result < DBL_MAX ? result : 0;
}
bool Rect::contains(const Point & p) const
{
    return p.x() >= xmin() && p.x() <= xmax() && p.y() >= ymin() && p.y() <= ymax();
}
bool Rect::intersects(const Rect & r) const
{
    return r.contains(left_bottom) || r.contains(right_top) || contains(r.left_bottom) || contains(r.right_top);
}

std::ostream & operator<<(std::ostream & strm, const Rect & r)
{
    return strm << "{ " << r.left_bottom << ", " << r.right_top << " }";
}
bool Rect::operator==(const Rect & rhs) const
{
    return left_bottom == rhs.left_bottom &&
            right_top == rhs.right_top;
}
bool Rect::operator!=(const Rect & rhs) const
{
    return !(rhs == *this);
}
// const Point & Rect::bottom() const
//{
//     return left_bottom;
// }
// const Point & Rect::top() const
//{
//     return right_top;
// }
