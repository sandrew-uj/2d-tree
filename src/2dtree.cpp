#include "primitives.h"

#include <algorithm>
#include <cfloat>
#include <chrono>
#include <fstream>
#include <random>

namespace rbtree {

PointSet::PointSet(const std::string & filename)
{
    std::ifstream inn(filename);
    double x, y;
    while (inn) {
        inn >> x >> y;
        this->put(Point(x, y));
    }
}

bool PointSet::empty() const
{
    return points.empty();
}

std::size_t PointSet::size() const
{
    return points.size();
}

void PointSet::put(const Point & p)
{
    points.insert(p);
}

bool PointSet::contains(const Point & p) const
{
    return points.find(p) != points.end();
}

std::pair<PointSet::iterator, PointSet::iterator> PointSet::range(const Rect & rect) const
{
    return {iterator(*this, rect), iterator(*this, points.end(), rect)};
}

PointSet::iterator PointSet::begin() const
{
    return iterator(*this, points.begin());
}

PointSet::iterator PointSet::end() const
{
    return iterator(*this, points.end());
}

std::optional<Point> PointSet::nearest(const Point & p) const
{
    auto min_point = std::min_element(points.begin(), points.end(), [&p](Point a, Point b) { return p.distance(a) < p.distance(b); });
    if (min_point != points.end()) {
        return *min_point;
    }

    return {};
}

std::pair<PointSet::iterator, PointSet::iterator> PointSet::nearest(const Point & p, std::size_t k) const
{
    std::set<double> distances;
    k = std::min(size(), k);
    if (k == 0) {
        return {iterator(*this, points.end()), iterator(*this, points.end())};
    }
    for (const auto point : points) {
        if (distances.size() == k && *distances.rbegin() < point.distance(p)) {
            continue;
        }
        if (distances.size() == k) {
            distances.erase(--distances.end());
        }
        distances.emplace(point.distance(p));
    }

    double radius = *(distances.rbegin());

    return {iterator(*this, p, radius), iterator(*this, points.end(), p, radius)};
}
std::ostream & operator<<(std::ostream & strm, const PointSet & ps)
{
    strm << "{ ";
    for (const Point point : ps.points) {
        strm << point << std::endl;
    }
    strm << " }";
    return strm;
}

} // namespace rbtree

namespace kdtree {

PointSet::PointSet(const std::string & filename)
{
    std::ifstream inn(filename);
    double x, y;
    std::vector<Point> data;
    while (inn) {
        inn >> x >> y;
        data.push_back(Point(x, y));
    }
    for (const Point point : data) {
        this->put(point);
    }
}

bool PointSet::empty() const
{
    return !m_root;
}

std::size_t PointSet::size() const
{
    return m_size;
}

void PointSet::put(const Point & point)
{
    if (contains(point)) {
        return;
    }
    if (m_root) {
        put(m_root, point, true, m_end, true);
        m_begin = m_root;
        while (m_begin->m_left) {
            m_begin = m_begin->m_left;
        }
    }
    else {
        m_begin = m_root = std::make_shared<Node>(point, true, m_end);
    }
    m_size++;
}

void PointSet::put(std::shared_ptr<Node> & node, const Point & point, bool is_x, const std::shared_ptr<Node> & next, bool is_balance) const
{
    node->m++;
    if ((point.x() < node->m_point.x() && node->is_x) || ((point.y() < node->m_point.y()) && !node->is_x)) {
        if (node->m_left) {
            put(node->m_left, point, !is_x, node, is_balance);
        }
        else {
            node->m_left = std::make_shared<Node>(point, !is_x, node);
        }
        if (is_balance && node->m_left->m > alpha * node->m) {
            balance(node, is_x);
        }
    }
    else {
        if (node->m_right) {
            put(node->m_right, point, !is_x, next, is_balance);
            node->m_next = node->m_right;
            while (node->m_next.lock()->m_left) {
                node->m_next = node->m_next.lock()->m_left;
            }
        }
        else {
            node->m_next = node->m_right = std::make_shared<Node>(point, !is_x, next);
        }
        if (is_balance && node->m_right->m > alpha * node->m) {
            balance(node, is_x);
        }
    }
}

void PointSet::build_tree(std::vector<std::shared_ptr<Node>> & temp, std::shared_ptr<Node> & root, bool is_x, std::shared_ptr<Node> & end, size_t left, size_t right) const
{
    size_t mid = (left + right) / 2;
    if (mid >= left && mid < right) {
        if (root) {
            put(root, temp[mid]->m_point, is_x, end, false);
        }
        else {
            root = std::make_shared<Node>(temp[mid]->m_point, is_x, end);
        }
        build_tree(temp, root, is_x, end, left, mid);
        build_tree(temp, root, is_x, end, mid + 1, right);
    }
}

void PointSet::balance(std::shared_ptr<Node> & node, bool is_x) const
{
    auto begin = node;
    while (begin->m_left) {
        begin = begin->m_left;
    }

    auto end = node;
    while (end->m_right) {
        end = end->m_right;
    }

    std::vector<std::shared_ptr<Node>> temp;
    temp.push_back(begin);
    do {
        begin = begin->m_next.lock();
        temp.push_back(begin);
    } while (begin.get() != end.get());

    end = end->m_next.lock();
    node.reset();
    build_tree(temp, node, is_x, end, 0, temp.size());
}

bool PointSet::contains(const Point & point) const
{
    return contains(m_root, point);
}

bool PointSet::contains(const std::shared_ptr<Node> & node, const Point & point) const
{
    if (!node) {
        return false;
    }

    if (node->m_point == point) {
        return true;
    }

    if ((point.x() < node->m_point.x() && node->is_x) || (point.y() < node->m_point.y() && !node->is_x)) {
        return contains(node->m_left, point);
    }
    else {
        return contains(node->m_right, point);
    }
}

std::pair<PointSet::iterator, PointSet::iterator> PointSet::range(const Rect & rect) const
{
    std::shared_ptr<Node> range_root, range_begin, range_end;
    if (!empty()) {
        range(m_root, rect, range_root, range_begin, range_end);
    }

    return {iterator(range_begin, range_root), iterator(range_end)};
}

void PointSet::range(const std::shared_ptr<Node> & node, const Rect & rect, std::shared_ptr<Node> & range_root, std::shared_ptr<Node> & range_begin, std::shared_ptr<Node> & range_end) const
{
    if (!node) {
        return;
    }

    if (rect.contains(node->m_point)) {
        if (range_root) {
            put(range_root, node->m_point, true, range_end, true);
            range_begin = range_root;
            while (range_begin->m_left) {
                range_begin = range_begin->m_left;
            }
        }
        else {
            range_root = std::make_shared<Node>(node->m_point, true, range_end);
            range_begin = range_root;
        }
    }

    double value = node->is_x ? node->m_point.x() : node->m_point.y(),
           max_dim = node->is_x ? rect.xmax() : rect.ymax(),
           min_dim = node->is_x ? rect.xmin() : rect.ymin();

    if (value < min_dim) {
        range(node->m_right, rect, range_root, range_begin, range_end);
    }
    else if (value > max_dim) {
        range(node->m_left, rect, range_root, range_begin, range_end);
    }
    else {
        range(node->m_left, rect, range_root, range_begin, range_end);
        range(node->m_right, rect, range_root, range_begin, range_end);
    }
}

PointSet::iterator PointSet::begin() const
{
    return iterator(m_begin);
}

PointSet::iterator PointSet::end() const
{
    return iterator(m_end);
}

std::optional<Point> PointSet::nearest(const Point & point) const
{
    auto [begin, end] = nearest(point, 1);
    if (begin != end) {
        return *begin;
    }
    return {};
}

std::pair<PointSet::iterator, PointSet::iterator> PointSet::nearest(const Point & point, std::size_t k) const
{
    std::shared_ptr<Node> nearest_root, nearest_begin, nearest_end;
    std::map<double, std::shared_ptr<Node>> node_dists;
    nearest(m_root, point, k, 0, true, node_dists, std::numeric_limits<double>::max());

    for (auto it = node_dists.begin(); it != node_dists.end(); it++) {
        if (nearest_root) {
            put(nearest_root, it->second->m_point, true, nearest_end, true);
            nearest_begin = nearest_root;
            while (nearest_begin->m_left) {
                nearest_begin = nearest_begin->m_left;
            }
        }
        else {
            nearest_root = std::make_shared<Node>(it->second->m_point, true, nearest_end);
            nearest_begin = nearest_root;
        }
    }

    return {iterator(nearest_begin, nearest_root), iterator(nearest_end)};
}

void PointSet::nearest(const std::shared_ptr<Node> & node, const Point & point, size_t k, size_t i, bool is_x, std::map<double, std::shared_ptr<Node>> & node_dists, double min_dist) const
{
    if (!node) {
        return;
    }

    double dist = point.distance(node->m_point);

    if (k > 0 && node_dists.size() == k && node_dists.rbegin()->first > dist) {
        node_dists.erase(node_dists.rbegin()->first);
    }
    if (node_dists.size() < k) {
        node_dists.insert({dist, node});
    }

    if (min_dist > dist) {
        min_dist = dist;
    }
    i++;

    double sub = is_x ? node->m_point.x() - point.x() : node->m_point.y() - point.y();
    std::shared_ptr<Node> node1(sub < 0 ? node->m_right : node->m_left);
    std::shared_ptr<Node> node2(sub >= 0 ? node->m_right : node->m_left);

    nearest(node1, point, k, i, !is_x, node_dists, min_dist);
    if (std::abs(sub) < min_dist || i < k) {
        nearest(node2, point, k, i, !is_x, node_dists, min_dist);
    }
}

std::ostream & operator<<(std::ostream & os, const PointSet & p)
{
    for (auto it = p.begin(); it != p.end(); ++it) {
        os << *it << "\n";
    }
    return os << std::endl;
}

} // namespace kdtree