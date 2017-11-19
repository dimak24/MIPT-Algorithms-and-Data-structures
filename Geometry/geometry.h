#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <cassert>
#include <algorithm>
#include <typeinfo>

const double eps = 0.0000000001;

struct Point {
    Point() {}

    double x, y;

    Point(const double& x0, const double& y0) : x(x0), y(y0) {}

    bool operator==(const Point& p) const {
        return abs(x - p.x) <= eps && abs(y - p.y) <= eps;
    }

    bool operator!=(const Point& p) const {
        return !(*this == p);
    }
};

double triangle_area(const Point&, const Point&, const Point&);

bool counter_clockwise(const Point&, const Point&, const Point&);

double distance_between_points(const Point&, const Point&);  

const Point middle(const Point&, const Point&);

class Line {
public:
    double A, B, C;

    Line(const Point& p1, const Point& p2) {
        A = p2.y - p1.y;
        B = p1.x - p2.x;
        C = p2.x * p1.y - p1.x * p2.y;
    }

    Line(const double& slope, const double& shift) {
        A = slope;
        B = -1.0;
        C = shift;
    }

    Line(const Point& p, const double& slope) {
        A = slope;
        B = 1.0;
        C = p.y - slope*p.x;
    }

    bool operator==(const Line& line) const {
        return abs(A * line.B - B * line.A) <= eps && abs(A * line.C - C * line.A) <= eps;
    }

    bool operator!=(const Line& line) const {
        return !(*this == line);
    }
};

double angle_between(const Line&, const Line&);

bool lines_intersection(const Line&, const Line&, Point&);

class PointOperations {
public:
    static void reflex(Point& point, const Line& axis) {
        double tmp = point.y;
        point.y = ((axis.A * axis.A - axis.B * axis.B) * point.y - 2.0 * axis.A * axis.B * point.x - 2.0 * axis.B * axis.C) / (axis.A * axis.A + axis.B * axis.B);
        point.x = ((axis.B * axis.B - axis.A * axis.A) * point.x - 2.0 * axis.A * axis.B * tmp - 2.0 * axis.A * axis.C) / (axis.A * axis.A + axis.B * axis.B);
    }

    static void rotate(Point& point, const Point& center, const double& angle) {
        Point p(point.x - center.x, point.y - center.y);
        double tmp = p.y;
        double alpha = (angle / 180.0) * M_PI;
        p.y = p.x * sin(alpha) + p.y * cos(alpha) + center.y;
        p.x = p.x * cos(alpha) - tmp * sin(alpha) + center.x;
        point.x = p.x;
        point.y = p.y;
    }

    static void reflex(Point& point, const Point& center) {
        point.x = center.x * 2.0 - point.x;
        point.y = center.y * 2.0 - point.y;
    }

    static void scale(Point& point, const Point& center, const double& coefficient) {
        point.x += (coefficient - 1.0) * (point.x - center.x);
        point.y += (coefficient - 1.0) * (point.y - center.y);
    }
};

class Shape {
public:
    virtual double perimeter() const = 0;

    virtual double area() const = 0;

    virtual bool operator==(const Shape& another) const = 0;

    virtual bool operator!=(const Shape& another) const = 0;

    virtual bool isCongruentTo(const Shape& another) const = 0;

    virtual bool isSimilarTo(const Shape& another) const = 0;

    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& center, const double& angle) = 0;

    virtual void reflex(const Point& center) = 0;

    virtual void reflex(const Line& axis) = 0;

    virtual void scale(const Point& center, const double& coefficient) = 0;

    virtual ~Shape() = default;
};

class Ellipse : public Shape {
protected:
    Point focus1, focus2;
    double sum_of_distances;
    Ellipse() {}

public:
    Ellipse(const Point& f1, const Point& f2, const double& sum) : focus1(f1), focus2(f2), sum_of_distances(sum) {
        assert(distance_between_points(f1, f2) < sum);
    }

    const std::pair<Point, Point> focuses() const {
        return { focus1, focus2 };
    }

    const std::pair<double, double> semiaxes() const {
        double a = sum_of_distances / 2.0;
        double b = distance_between_points(focus1, focus2);
        b = sqrt(sum_of_distances * sum_of_distances - b * b) / 2.0;
        return { a, b };
    }

    double eccentricity() const {
        std::pair<double, double> axes = semiaxes();
        return sqrt(axes.first * axes.first - axes.second * axes.second) / axes.first;
    }

    const Point center() const {
        return middle(focus1, focus2);
    }

    bool containsPoint(const Point& point) const {
        return distance_between_points(point, focus1) + distance_between_points(point, focus2) <= sum_of_distances;
    }

    double area() const {
        std::pair<double, double> axes = semiaxes();
        return M_PI * axes.first * axes.second;
    }

    double perimeter() const {
        std::pair<double, double> axes = semiaxes();
        double c = 3.0 * (axes.first - axes.second) * (axes.first - axes.second) / ((axes.first + axes.second) * (axes.first + axes.second));
        return M_PI * (axes.first + axes.second) * (1.0 + c / (10.0 + sqrt(4.0 - c)));
    }

    bool operator==(const Shape& another) const {
        try {
            Ellipse ellipse = dynamic_cast<Ellipse&>(const_cast<Shape&>(another));
            return ((focus1 == ellipse.focus1 && focus2 == ellipse.focus2) || (focus1 == ellipse.focus2 && focus2 == ellipse.focus1))
                && abs(sum_of_distances - ellipse.sum_of_distances) <= eps;
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    bool operator!=(const Shape& another) const {
        return !(*this == another);
    }

    void scale(const Point& center, const double& coefficient) {
        assert(coefficient != 0.0);
        PointOperations::scale(focus1, center, coefficient);
        PointOperations::scale(focus2, center, coefficient);
        sum_of_distances *= abs(coefficient);
    }

    void rotate(const Point& center, const double& angle) {
        PointOperations::rotate(focus1, center, angle);
        PointOperations::rotate(focus2, center, angle);
    }

    void reflex(const Point& center) {
        PointOperations::reflex(focus1, center);
        PointOperations::reflex(focus2, center);
    }

    void reflex(const Line& axis) {
        PointOperations::reflex(focus1, axis);
        PointOperations::reflex(focus2, axis);
    }

    bool isSimilarTo(const Shape& another) const {
        try {
            Ellipse ellipse = dynamic_cast<Ellipse&>(const_cast<Shape&>(another));
            std::pair<double, double> axes = semiaxes(), another_axes = ellipse.semiaxes();
            return abs(axes.first * another_axes.second - axes.second * another_axes.first) <= eps;
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    bool isCongruentTo(const Shape& another) const {
        try {
            Ellipse ellipse = dynamic_cast<Ellipse&>(const_cast<Shape&>(another));
            return isSimilarTo(another) && abs(sum_of_distances - ellipse.sum_of_distances) <= eps;
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    const std::pair<Line, Line> directrixes() const {
        Line big_axis(focus1, focus2);
        Point c = center();
        std::pair<double, double> axes = semiaxes();
        double dist = abs(axes.second / axes.first);
        if (abs(big_axis.B) <= eps)
            return { Line(Point(-big_axis.C / big_axis.A, c.y + dist), 0.0), Line(Point(-big_axis.C / big_axis.A, c.y - dist), 0.0) };
        if (abs(big_axis.A) <= eps)
            return { Line(Point(c.x + dist, 0.0), Point(c.x + dist, 1.0)), Line(Point(c.x - dist, 0.0), Point(c.x - dist, 1.0)) };
        double k = -big_axis.B / big_axis.A;
        double sin = k / sqrt(k * k + 1.0), cos = 1.0 / sqrt(k * k + 1.0);
        return { Line(Point(c.x + dist * cos, c.y + dist * sin), -1.0 / k), Line(Point(c.x - dist * cos, c.y - dist * sin), -1.0 / k) };
    }
};

class Circle : public Ellipse {
public:
    const std::pair<Line, Line> directrixes() = delete;

    Circle(const Point& center, const double& radius) {
        assert(radius > 0);
        focus1 = focus2 = center;
        sum_of_distances = radius * 2.0;
    }

    double radius() const {
        return sum_of_distances / 2.0;
    }
};

class Polygon : public Shape {
protected:
    std::vector<Point> vertices;
    Polygon() {}

public:
    template <class...Point>
    Polygon(const Point&...points) : vertices({ points... }) {}

    Polygon(const std::vector<Point>& points) : vertices(points.begin(), points.end()) {}

    size_t verticesCount() const {
        return vertices.size();
    }

    const std::vector<Point> getVertices() const {
        return std::vector<Point>(vertices);
    }

    bool isConvex() const {
        size_t size = verticesCount();
        bool ans = counter_clockwise(vertices[0], vertices[1], vertices[2]);
        for (size_t i = 1; i < size; ++i)
            if (counter_clockwise(vertices[i], vertices[(i + 1) % size], vertices[(i + 2) % size]) ^ ans)
                return false;
        return true;
    }

    double perimeter() const {
        const size_t size = verticesCount();
        double ans = distance_between_points(vertices[size - 1], vertices[0]);
        for (size_t i = 0; i < size - 1; ++i)
            ans += distance_between_points(vertices[i], vertices[i + 1]);
        return ans;
    }

    double area() const {
        double ans = 0;
        for (size_t i = 1; i < verticesCount() - 1; ++i)
            ans += triangle_area(vertices[0], vertices[i], vertices[i + 1]);
        return fabs(ans);
    }
    
    void rotate(const Point& center, const double& angle) {
        for (size_t i = 0; i < verticesCount(); ++i)
            PointOperations::rotate(vertices[i], center, angle);
    }

    void reflex(const Point& center) {
        for (size_t i = 0; i < verticesCount(); ++i)
            PointOperations::reflex(vertices[i], center);
    }

    void reflex(const Line& axis) {
        for (size_t i = 0; i < verticesCount(); ++i)
            PointOperations::reflex(vertices[i], axis);
    }

    void scale(const Point& center, const double& coefficient) {
        assert(coefficient != 0.0);
        for (size_t i = 0; i < verticesCount(); ++i)
            PointOperations::scale(vertices[i], center, coefficient);
    }

    bool operator==(const Shape& another) const {
        try {
            Polygon polygon = dynamic_cast<Polygon&>(const_cast<Shape&>(another));
            if (polygon.verticesCount() != verticesCount())
                return false;
            int start = -1;
            size_t n = verticesCount();
            for (size_t i = 0; i < n; ++i)
                if (polygon.vertices[i] == vertices[0]) {
                    start = i;
                    break;
                }
            if (start == -1)
                return false;
            if (polygon.vertices[(start + 1) % n] == vertices[1]) {
                for (size_t i = 2; i < n; ++i)
                    if (polygon.vertices[(start + i) % n] != vertices[i])
                        return false;
                return true;
            }
            else if (polygon.vertices[(start - 1 + n) % n] == vertices[1]) {
                for (size_t i = 2; i < n; ++i)
                    if (polygon.vertices[(start - i + n) % n] != vertices[i])
                        return false;
                return true;
            }
            return false;
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    bool operator!=(const Shape& another) const {
        return !(*this == another);
    }

    bool isSimilarTo(const Shape& another) const {
        try {
            Polygon polygon = dynamic_cast<Polygon&>(const_cast<Shape&>(another));
            if (verticesCount() != polygon.verticesCount())
                return false;
            int n = verticesCount();
            polygon.scale(Point(0, 0), perimeter() / polygon.perimeter());
            for (int symmemtry = 0; symmemtry <= 1; ++symmemtry) {
                for (int numeration = 0; numeration <= 1; ++numeration) {
                    for (int i = 0; i < n; ++i) {
                        bool ans = true;
                        for (int j = 0; j < n; ++j) {
                            if (abs(distance_between_points(vertices[j], vertices[(j + 1) % n]) -
                                distance_between_points(polygon.vertices[(j + i) % n], polygon.vertices[(j + i + 1) % n])) > eps)
                                ans = false;
                        }
                        if (!ans)
                            continue;
                        double x = vertices[0].x - polygon.vertices[i].x, y = vertices[0].y - polygon.vertices[i].y;
                        for (int j = 0; j < n; ++j) {
                            polygon.vertices[j].x += x;
                            polygon.vertices[j].y += y;
                        }
                        double angle = angle_between(Line(vertices[0], vertices[1]), Line(polygon.vertices[i], polygon.vertices[(i + 1) % n]));
                        angle = (angle / M_PI) * 180.0;
                        polygon.rotate(vertices[0], -angle);
                        if (polygon == *this)
                            return true;
                        polygon.rotate(vertices[0], 2.0 * angle);
                        if (polygon == *this)
                            return true;
                    }
                    for (int i = 0; i < n / 2; ++i)
                        std::swap(polygon.vertices[i], polygon.vertices[n - 1 - i]);
                }
                polygon.reflex(Line(Point(0, 0), Point(1, 0)));
            }
            return false;
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    bool isCongruentTo(const Shape& another) const {
        try {
            return isSimilarTo(another) && (abs(another.perimeter() - perimeter()) <= eps);
        }
        catch (std::bad_cast) {
            return false;
        }
    }

    bool containsPoint(const Point& point) const {
        double y0 = point.y;
        Line l(0, y0);
        Point p(0, 0);
        size_t n = verticesCount();
        bool ans = false;
        for (size_t i = 0; i < n; ++i) {
            Point v1 = vertices[i], v2 = vertices[(i + 1) % n];
            if(lines_intersection(Line(v1, v2), l, p) && p.x <= point.x)
                ans ^= ((v1.y >= y0) ^ (v2.y >= y0));
        }
        return ans;
    }
};

class Rectangle : public Polygon {
protected:
    Rectangle() {}

public:
    Rectangle(const Point& v1, const Point& v2, const double& sides_ratio) {
        double alpha = -atan(std::min(sides_ratio, 1.0 / sides_ratio)) * 2.0;
        alpha = (alpha / M_PI) * 180.0;
        Point v3 = v1, v4 = v2;
        PointOperations::rotate(v3, middle(v1, v2), alpha);
        PointOperations::rotate(v4, middle(v1, v2), alpha);
        vertices.assign({ v1,v3,v2,v4 });
    }

    const std::pair<Line, Line> diagonals() const {
        return { Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3]) };
    }

    const Point center() const {
        return middle(vertices[0], vertices[2]);
    }
};

class Square : public Rectangle {
public:
    Square(const Point& v1, const Point& v2) {
        Rectangle sq(v1, v2, 1.0);
        std::vector<Point> v = sq.getVertices();
        vertices.assign(v.begin(), v.end());
    }

    const Circle circumscribedCircle() const {
        return Circle(center(), distance_between_points(vertices[0], vertices[2]) / 2.0);
    }

    const Circle inscribedCircle() const {
        return Circle(center(), distance_between_points(vertices[0], vertices[1]) / 2.0);
    }
};

class Triangle : public Polygon {
public:
    Triangle(const Point& v1, const Point& v2, const Point& v3) {
        vertices.assign({ v1, v2, v3 });
    }

    const Circle circumscribedCircle() const {
        double a = distance_between_points(vertices[1], vertices[2]), b = distance_between_points(vertices[0], vertices[2]), c = distance_between_points(vertices[0], vertices[1]);
        double R = a * b * c / (area() * 4.0);
        double x = a * a * (a * a - b * b - c * c), y = b * b * (b * b - a * a - c * c), z = c * c * (c * c - a * a - b * b);
        double s = x + y + z;
        return Circle(Point((vertices[0].x * x + vertices[1].x * y + vertices[2].x * z) / s, (vertices[0].y * x + vertices[1].y * y + vertices[2].y * z) / s), R);
    }

    const Circle inscribedCircle() const {
        double x = distance_between_points(vertices[1], vertices[2]), y = distance_between_points(vertices[0], vertices[2]), z = distance_between_points(vertices[0], vertices[1]);
        double r = 2.0 * area() / perimeter();
        double s = x + y + z;
        return Circle(Point((vertices[0].x * x + vertices[1].x * y + vertices[2].x * z) / s, (vertices[0].y * x + vertices[1].y * y + vertices[2].y * z) / s), r);
    }

    const Point centroid() const {
        return Point((vertices[0].x + vertices[1].x + vertices[2].x) / 3.0, (vertices[0].y + vertices[1].y + vertices[2].y) / 3.0);
    }

    const Point orthocenter() const {
        Point O = circumscribedCircle().center();
        PointOperations::scale(O, centroid(), -2.0);
        return O;
    }

    const Line EulerLine() const {
        return Line(centroid(), orthocenter());
    }

    const Circle ninePointsCircle() const {
        Circle ans = circumscribedCircle();
        ans.scale(orthocenter(), 0.5);
        return ans;
    }
};

double triangle_area(const Point& p1, const Point& p2, const Point& p3) {
    return (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0;
}

bool counter_clockwise(const Point& p1, const Point& p2, const Point& p3) {
    return triangle_area(p1, p2, p3) > 0;
}

double distance_between_points(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

const Point middle(const Point& p1, const Point& p2) {
    return Point((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
}

double angle_between(const Line& l1, const Line& l2) {
    return acos((l1.A * l2.A + l1.B * l2.B) / sqrt((l1.A * l1.A + l1.B * l1.B) * (l2.A * l2.A + l2.B * l2.B)));
}

bool lines_intersection(const Line& l1, const Line& l2, Point& ans) {
    if (abs(l1.B * l2.A - l1.A - l2.B) <= eps)
        return false;
    ans.x = (-l2.B * l1.C + l2.C*l1.B) / (l1.B * l2.A - l1.A - l2.B);
    ans.y = (-l1.A * l2.C + l2.A*l1.C) / (l1.B * l2.A - l1.A - l2.B);
    return true;
}