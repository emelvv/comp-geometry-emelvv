#include <bits/stdc++.h>

using namespace std;

constexpr double eps = 1e-9;

struct Point {
    double x = 0;
    double y = 0;

    Point() = default;
    Point(double x_value, double y_value) : x(x_value), y(y_value) {
    }

    Point operator+(const Point& other) const {
        return { x + other.x, y + other.y };
    }

    Point operator-(const Point& other) const {
        return { x - other.x, y - other.y };
    }

    Point operator*(double factor) const {
        return { x * factor, y * factor };
    }
};

bool same_point(const Point& first, const Point& second) {
    return fabs(first.x - second.x) <= eps && fabs(first.y - second.y) <= eps;
}

double polygon_area(const vector<Point>& polygon) {
    double area2 = 0;
    int size = static_cast<int>(polygon.size());
    for (int i = 0; i < size; ++i) {
        const Point& current = polygon[i];
        const Point& next = polygon[(i + 1) % size];
        area2 += current.x * next.y - current.y * next.x;
    }
    return fabs(area2) * 0.5;
}

vector<Point> clip_with_halfplane(const vector<Point>& polygon, double a, double b, double c) {
    vector<Point> result;
    if (polygon.empty()) {
        return result;
    }

    auto value = [&](const Point& point) {
        return a * point.x + b * point.y + c;
    };

    int size = static_cast<int>(polygon.size());
    result.reserve(size + 1);
    for (int i = 0; i < size; ++i) {
        const Point& current = polygon[i];
        const Point& next = polygon[(i + 1) % size];
        double current_value = value(current);
        double next_value = value(next);
        bool current_inside = current_value <= eps;
        bool next_inside = next_value <= eps;

        if (current_inside && next_inside) {
            result.push_back(next);
            continue;
        }

        if (current_inside != next_inside) {
            double t = current_value / (current_value - next_value);
            result.push_back(current + (next - current) * t);
        }

        if (!current_inside && next_inside) {
            result.push_back(next);
        }
    }

    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    double w, h;
    cin >> w >> h;

    vector<Point> points(n);
    for (Point& point : points) {
        cin >> point.x >> point.y;
    }

    cout << fixed << setprecision(10);
    for (int i = 0; i < n; ++i) {
        vector<Point> cell = {
            { 0.0, 0.0 },
            { w, 0.0 },
            { w, h },
            { 0.0, h }
        };

        for (int j = 0; j < n && !cell.empty(); ++j) {
            if (i == j) {
                continue;
            }

            if (same_point(points[i], points[j])) {
                cell.clear();
                break;
            }

            double a = 2.0 * (points[j].x - points[i].x);
            double b = 2.0 * (points[j].y - points[i].y);
            double c = points[i].x * points[i].x + points[i].y * points[i].y -
                points[j].x * points[j].x - points[j].y * points[j].y;

            cell = clip_with_halfplane(cell, a, b, c);
        }

        cout << polygon_area(cell) << '\n';
    }

    return 0;
}
