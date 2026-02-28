#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

struct Point {
    int64_t x, y;
};

bool pointLess(const Point& lhs, const Point& rhs) {
    if (lhs.x != rhs.x)
    {
        return lhs.x < rhs.x;
    }
    return lhs.y < rhs.y;
}

bool pointEqual(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

int64_t cross(const Point& a, const Point& b, const Point& c) {
    return  (b.x - a.x) * (c.y - a.y) -
        (b.y - a.y) * (c.x - a.x);
}

int64_t dist2(const Point& a, const Point& b) {
    int64_t dx = a.x - b.x;
    int64_t dy = a.y - b.y;
    return dx * dx + dy * dy;
}

vector<Point> buildLowerHull(const vector<Point>* points) {
    if (points == nullptr || points->empty()) {
        return {};
    }

    vector<Point> lower;
    lower.reserve(points->size());

    for (const Point& p : *points) {
        while (lower.size() >= 2 &&
            cross(lower[lower.size() - 2], lower.back(), p) <= 0.0) {
            lower.pop_back();
        }
        lower.push_back(p);
    }

    return lower;
}

vector<Point> buildUpperHull(const vector<Point>* points) {
    if (points == nullptr || points->empty()) {
        return {};
    }

    vector<Point> upper;
    upper.reserve(points->size());

    for (int i = static_cast<int>(points->size()) - 1; i >= 0; --i) {
        const Point& p = (*points)[i];
        while (upper.size() >= 2 &&
            cross(upper[upper.size() - 2], upper.back(), p) <= 0.0) {
            upper.pop_back();
        }
        upper.push_back(p);
    }

    return upper;
}

vector<Point> buildConvexHull(const vector<Point>* points) {
    if (points == nullptr || points->empty()) {
        return {};
    }

    if (points->size() == 1) {
        return { (*points)[0] };
    }

    vector<Point> lower = buildLowerHull(points);
    vector<Point> upper = buildUpperHull(points);

    lower.pop_back();
    upper.pop_back();

    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

int64_t diameterSquared(const vector<Point>& hull) {
    int m = hull.size();
    if (m <= 1)
    {
        return 0;
    }
    if (m == 2)
    {
        return  dist2(hull[0], hull[1]);
    }

    int j = 1;
    int64_t best = 0;

    for (int i = 0; i < m; ++i) {
        int ni = (i + 1) % m;
        while (true) {
            int nj = (j + 1) % m;
            int64_t cur = cross(hull[i], hull[ni], hull[nj]);
            int64_t prev = cross(hull[i], hull[ni], hull[j]);
            if (cur > prev) {
                j = nj;
            }
            else {
                break;
            }
        }

        best = max(best, dist2(hull[i], hull[j]));
        best = max(best, dist2(hull[ni], hull[j]));
    }

    return  best;
}

int main() {
    int n;
    cin >> n;
    vector<Point> pts(n);
    for (int i = 0; i < n; ++i) {
        cin >> pts[i].x >> pts[i].y;
    }

    sort(pts.begin(), pts.end(), pointLess);
    pts.erase(unique(pts.begin(), pts.end(), pointEqual), pts.end());

    vector<Point> hull = buildConvexHull(&pts);
    cout << diameterSquared(hull) << '\n';
    return 0;
}
