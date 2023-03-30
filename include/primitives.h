#pragma once
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

class Point
{
public:
    Point() = default;

    Point(double x, double y);

    double x() const;
    double y() const;
    double distance(const Point &) const;

    bool operator<(const Point &) const;
    bool operator>(const Point &) const;
    bool operator<=(const Point &) const;
    bool operator>=(const Point &) const;
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;

    bool equals_d(double a, double b) const;

    friend std::ostream & operator<<(std::ostream &, const Point &);

private:
    double px = std::numeric_limits<double>::max(), py = std::numeric_limits<double>::max();
};

class Rect
{
public:
    Rect() = default;

    Rect(const Point & left_bottom, const Point & right_top);

    double xmin() const;
    double ymin() const;
    double xmax() const;
    double ymax() const;
    double distance(const Point & p) const;

    bool contains(const Point & p) const;
    bool intersects(const Rect &) const;

    bool operator==(const Rect & rhs) const;
    bool operator!=(const Rect & rhs) const;

    friend std::ostream & operator<<(std::ostream &, const Rect &);

private:
    Point left_bottom, right_top;
};

namespace rbtree {

class PointSet
{
public:
    using set_iterator = std::set<Point>::iterator;

    class iterator
    {
    public:
        iterator() = default;

        using difference_type = std::ptrdiff_t;
        using value_type = Point;
        using pointer = const value_type *;
        using reference = const value_type &;
        using iterator_category = std::forward_iterator_tag;

        bool operator==(const PointSet::iterator & rhs) const
        {
            return m_current == rhs.m_current;
        }
        bool operator!=(const PointSet::iterator & rhs) const
        {
            return !(rhs == *this);
        }

        reference operator*() const
        {
            return *m_current;
        }

        pointer operator->() const
        {
            return &(*m_current);
        }

        iterator & operator++()
        {
            next_point(m_current, m_rect, m_point, m_radius);
            return *this;
        }

        iterator operator++(int)
        {
            auto tmp = *this;
            operator++();
            return tmp;
        }

    private:
        friend class PointSet;

        iterator(const PointSet & ps, set_iterator it)
            : m_current(it)
            , m_end(ps.points.end())
        {
        }

        iterator(const PointSet & ps, set_iterator it, const Rect & rect)
            : m_current(it)
            , m_rect(rect)
            , m_end(ps.points.end())
        {
            if (it != ps.points.end() && !rect.contains(*it)) {
                (*this)++;
            }
        }

        iterator(const PointSet & ps, const Rect & rect)
            : iterator(ps, ps.points.begin(), rect)
        {
        }

        iterator(const PointSet & ps, set_iterator it, const Point & point, double const radius)
            : m_current(it)
            , m_point(point)
            , m_radius(radius)
            , m_end(ps.points.end())
        {
            if (it != ps.points.end() && point.distance(*it) > radius) {
                (*this)++;
            }
        }

        iterator(const PointSet & ps, const Point & point, double const radius)
            : iterator(ps, ps.points.begin(), point, radius)
        {
        }

        set_iterator next_point(set_iterator & it, const Rect & rect, const Point & point, double radius) const
        {
            if (rect != Rect()) {
                while (++it != m_end && !rect.contains(*it)) {
                }
            }
            else if (point != Point()) {
                while (++it != m_end && point.distance(*it) > radius) {
                }
            }
            else {
                ++it;
            }
            return it;
        }

        set_iterator m_current;
        //        const PointSet * m_ps = nullptr;
        Rect m_rect;
        Point m_point;
        double m_radius;
        set_iterator m_end;
    };

    PointSet(const std::string & filename = {});

    bool empty() const;
    std::size_t size() const;
    void put(const Point &);
    bool contains(const Point &) const;

    // second iterator points to an element out of range
    std::pair<iterator, iterator> range(const Rect &) const;
    iterator begin() const;
    iterator end() const;

    std::optional<Point> nearest(const Point &) const;
    // second iterator points to an element out of range
    std::pair<iterator, iterator> nearest(const Point & p, std::size_t k) const;

    friend std::ostream & operator<<(std::ostream &, const PointSet &);

private:
    std::set<Point> points;
};

} // namespace rbtree

namespace kdtree {

class PointSet
{
public:
    PointSet(const std::string & filename = {});

    struct Node
    {
        bool operator==(const Node & rhs) const
        {
            return m_point == rhs.m_point;
        }
        bool operator!=(const Node & rhs) const
        {
            return !(rhs == *this);
        }

        Node(const Point & p, bool is_x, const std::shared_ptr<Node> & next)
            : m_point(p)
            , m_next(next)
            , is_x(is_x)
        {
        }

        Point m_point;
        std::shared_ptr<Node> m_left, m_right;
        std::weak_ptr<Node> m_next;
        bool is_x = true;
        std::size_t m = 1;
    };

    class iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Point;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;

        iterator(std::shared_ptr<Node> node)
            : m_node(std::move(node))
        {
        }

        iterator(std::shared_ptr<Node> node, std::shared_ptr<Node> root)
            : m_node(std::move(node))
            , m_root(std::move(root))
        {
        }

        reference operator*()
        {
            return m_node->m_point;
        }

        pointer operator->()
        {
            return &m_node->m_point;
        }

        iterator & operator++()
        {
            m_node = m_node->m_next.lock();
            return *this;
        }

        iterator operator++(int)
        {
            iterator it = *this;
            ++*this;
            return it;
        }

        bool operator==(const iterator & it) const
        {
            return m_node == it.m_node;
        }

        bool operator!=(const iterator & it) const
        {
            return !(it == *this);
        }

        ~iterator() = default;

    private:
        std::shared_ptr<Node> m_node;
        std::shared_ptr<Node> m_root;
    };

    bool empty() const;
    std::size_t size() const;
    void put(const Point &);
    bool contains(const Point &) const;

    // second iterator points to an element out of range
    std::pair<iterator, iterator> range(const Rect &) const;
    iterator begin() const;
    iterator end() const;

    std::optional<Point> nearest(const Point &) const;
    // second iterator points to an element out of range
    std::pair<iterator, iterator> nearest(const Point & point, std::size_t k) const;
    //    virtual ~PointSet();
    friend std::ostream & operator<<(std::ostream &, const PointSet &);
    ~PointSet() = default;

private:
    const double alpha = .7;
    std::shared_ptr<Node> m_root, m_begin, m_end;
    int m_size = 0;

    void build_tree(std::vector<std::shared_ptr<Node>> & temp, std::shared_ptr<Node> & root, bool is_x, std::shared_ptr<Node> & end, size_t left, size_t right) const;
    void put(std::shared_ptr<Node> &, const Point &, bool, const std::shared_ptr<Node> &, bool) const;
    bool contains(const std::shared_ptr<Node> &, const Point &) const;
    void range(const std::shared_ptr<Node> &, const Rect &, std::shared_ptr<Node> &, std::shared_ptr<Node> &, std::shared_ptr<Node> &) const;
    void nearest(const std::shared_ptr<Node> &, const Point &, size_t, size_t, bool, std::map<double, std::shared_ptr<Node>> &, double) const;
    void balance(std::shared_ptr<Node> &, bool is_x) const;
};
} // namespace kdtree
