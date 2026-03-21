#include <bits/stdc++.h>
using namespace std;

constexpr double kEps = 1e-9;

struct Point {
    double x = 0;
    double y = 0;
};

Point operator+(const Point& lhs, const Point& rhs) {
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

Point operator-(const Point& lhs, const Point& rhs) {
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

double cross(const Point& lhs, const Point& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double cross(const Point& a, const Point& b, const Point& c) {
    return cross(b - a, c - a);
}

double polygon_area2(const vector<Point>& polygon) {
    double area2 = 0;
    int n = static_cast<int>(polygon.size());
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        area2 += cross(polygon[i], polygon[j]);
    }
    return area2;
}

bool lexicographically_lower(const Point& lhs, const Point& rhs) {
    if (abs(lhs.y - rhs.y) > kEps) {
        return lhs.y < rhs.y;
    }
    return lhs.x < rhs.x;
}

void normalize_polygon(vector<Point>& polygon) {
    if (polygon_area2(polygon) < 0) {
        reverse(polygon.begin(), polygon.end());
    }

    int start = 0;
    for (int i = 1; i < static_cast<int>(polygon.size()); ++i) {
        if (lexicographically_lower(polygon[i], polygon[start])) {
            start = i;
        }
    }
    rotate(polygon.begin(), polygon.begin() + start, polygon.end());
}

vector<Point> minkowski_sum(const vector<Point>& first, const vector<Point>& second) {
    int n = static_cast<int>(first.size());
    int m = static_cast<int>(second.size());

    vector<Point> ext_first = first;
    vector<Point> ext_second = second;
    ext_first.push_back(first[0]);
    ext_first.push_back(first[1]);
    ext_second.push_back(second[0]);
    ext_second.push_back(second[1]);

    vector<Point> result;
    result.reserve(n + m);
    result.push_back(first[0] + second[0]);

    int i = 0;
    int j = 0;
    while (i < n || j < m) {
        Point edge_first = ext_first[i + 1] - ext_first[i];
        Point edge_second = ext_second[j + 1] - ext_second[j];

        if (j == m || (i < n && cross(edge_first, edge_second) > kEps)) {
            result.push_back(result.back() + edge_first);
            ++i;
        }
        else if (i == n || (j < m && cross(edge_first, edge_second) < -kEps)) {
            result.push_back(result.back() + edge_second);
            ++j;
        }
        else {
            result.push_back(result.back() + edge_first + edge_second);
            ++i;
            ++j;
        }
    }

    result.pop_back();
    return result;
}

bool on_segment(const Point& p, const Point& a, const Point& b) {
    if (abs(cross(a, b, p)) > kEps) {
        return false;
    }
    return min(a.x, b.x) <= p.x + kEps && p.x <= max(a.x, b.x) + kEps &&
        min(a.y, b.y) <= p.y + kEps && p.y <= max(a.y, b.y) + kEps;
}

bool contains_zero(const vector<Point>& polygon) {
    Point origin{ 0, 0 };
    int n = static_cast<int>(polygon.size());
    for (int i = 0; i < n; ++i) {
        if (on_segment(origin, polygon[i], polygon[(i + 1) % n])) {
            return true;
        }
    }

    bool positive = false;
    bool negative = false;
    for (int i = 0; i < n; ++i) {
        double value = cross(polygon[i], polygon[(i + 1) % n], origin);
        if (value > kEps) {
            positive = true;
        }
        else if (value < -kEps) {
            negative = true;
        }
    }
    return !(positive && negative);
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<Point> first(n);
    for (Point& point : first) {
        cin >> point.x >> point.y;
    }

    int m;
    cin >> m;
    vector<Point> second(m);
    for (Point& point : second) {
        cin >> point.x >> point.y;
        point.x = -point.x;
        point.y = -point.y;
    }

    normalize_polygon(first);
    normalize_polygon(second);

    vector<Point> sum = minkowski_sum(first, second);
    cout << (contains_zero(sum) ? "YES" : "NO") << '\n';
    return 0;
}
