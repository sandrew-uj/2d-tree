#include "primitives.h"

#include <cmath>

Point::Point(double x, double y)
    : px(x)
    , py(y)
{
}
double Point::x() const
{
    return px;
}
double Point::y() const
{
    return py;
}
double Point::distance(const Point & p) const
{
    return std::sqrt((px - p.x()) * (px - p.x()) + (py - p.y()) * (py - p.y()));
}
bool Point::operator<(const Point & p) const
{
    return px < p.x() || (px == p.x() && py < p.y());
}
bool Point::operator>(const Point & p) const
{
    return px > p.x() || (px == p.x() && py > p.y());
}
bool Point::operator<=(const Point & p) const
{
    return !(*this > p);
}
bool Point::operator>=(const Point & p) const
{
    return !(*this < p);
}
bool Point::operator==(const Point & p) const
{
    return equals_d(px, p.x()) && equals_d(py, p.y());
}
bool Point::operator!=(const Point & p) const
{
    return !(*this == p);
}
std::ostream & operator<<(std::ostream & strm, const Point & p)
{
    return strm << "(" << p.x() << ", " << p.y() << ")";
}
bool Point::equals_d(double a, double b) const
{
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}
