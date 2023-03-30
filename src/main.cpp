#include "primitives.h"

#include <iostream>

void add_in_set(std::set<std::string> & s, std::string & el) {
    s.insert(el);
}

int main()
{


//    kdtree::PointSet ps1;
//    std::cout << "main";
//    ps1.put(Point(2,2));
//    std::cout << ps1.size() << " " << *ps1.begin() << std::endl;
//    std::set<std::string> strings;
//    std::string s = "asdlf";
//    add_in_set(strings, s);
//    s = "";
//    std::cout << *strings.begin();
//    std::set<double> my_set;
//    my_set.insert(1.);
//    my_set.insert(2.);
//    my_set.insert(3.);
//    my_set.erase(--my_set.end());
//    for(auto elem: my_set) {
//        std::cout << elem << " ";
//    }
//    std::cout << *my_set.rbegin();

    kdtree::PointSet ps1("C:\\Users\\DNS\\CplusplusProjects\\2d-tree-sandrew-uj\\test\\etc\\test2.dat");
//    std::cout << ps1.size() << std::endl;
    std::cout << ps1 << std::endl;
//    //    std::cout << ps1.contains(Point(0.753, 0.943)) << std::endl;
    //    Rect r1 = Rect(Point(0.634, 0.276), Point(.818, .42));
    //    std::cout << std::boolalpha << r1.contains(Point(0.815, 0.839));
    //    Point point(.386, .759);
    //    auto range = ps1.nearest(point, 210);
    //    std::cout << *(range.first);
    //    int size = 0;
    //    for(auto it = range.first; it != range.second; it++) {
    //        std::cout << *it << " ";
    //        size++;
    //    }
    //    std::cout << size << std::endl;
    //    auto range = ps1.nearest(Point(.386, .759), 0);
    //    auto it1 = rbtree::PointSet::iterator();
    //    it1 = ps1.nearest(Point(.386, .759), 0).first;
    //    std::cout << std::boolalpha << (Point(9, 7) == Point(1, 2));
    //    ps1.put(Point(1, 2));
    //    ps1.put(Point(3, 4));
    //    std::cout << ps1 << std::endl;
    //    std::cout << std::boolalpha << ps1.contains(Point(1, 2)) << " " << ps1.contains(Point(7, 7));
    //    std::cout << ps1.nearest(Point(7, 8)).value() << std::endl;

    //    Point p1(1, 2), p2(1, 2), p3(4, 6), p4(13, 7);
    //    p2 = p1;
    //    Rect r1(p1, p3), r2(p1, p4);
    //    std::cout << r1 << std::endl << r2 << std::endl << std::boolalpha
    //              << r2.contains(p3) << " " << r2.xmin() << " " << r2.ymin()
    //              << " " << r2.xmax() << " " << r2.ymax() << r1.intersects(r2) << r2.intersects(r1) << std::endl;
    //    std::cout << p1 << " " << p2 << " " << p3 << " "
    //              << (p1 == p2) << " " << p2.distance(p3) << " " << p3.distance(p2) << std::endl;
}
