#include <bits/stdc++.h>
using namespace std;

const double PI = acos(-1.0);

struct Point {
  double x;
  double y;
};

struct Segment {
  Point p1;
  Point p2;
};

struct Line {
  Point p;
  Point dir;
};

double cross(const Point &a, const Point &b, const Point &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

double dist(const Point &a, const Point &b) {
  return hypot(a.x - b.x, a.y - b.y);
}

bool samePoint(const Point &a, const Point &b) {
  return a.x == b.x && a.y == b.y;
}

vector<Point> buildLowerHull(const vector<Point> *points) {
  if (points == nullptr || points->empty()) {
    return {};
  }

  vector<Point> lower;
  lower.reserve(points->size());

  for (const Point &p : *points) {
    while (lower.size() >= 2 &&
           cross(lower[lower.size() - 2], lower.back(), p) <= 0.0) {
      lower.pop_back();
    }
    lower.push_back(p);
  }

  return lower;
}

vector<Point> buildUpperHull(const vector<Point> *points) {
  if (points == nullptr || points->empty()) {
    return {};
  }

  vector<Point> upper;
  upper.reserve(points->size());

  for (int i = static_cast<int>(points->size()) - 1; i >= 0; --i) {
    const Point &p = (*points)[i];
    while (upper.size() >= 2 &&
           cross(upper[upper.size() - 2], upper.back(), p) <= 0.0) {
      upper.pop_back();
    }
    upper.push_back(p);
  }

  return upper;
}

vector<Point> buildConvexHull(const vector<Point> *points) {
  if (points == nullptr || points->empty()) {
    return {};
  }

  if (points->size() == 1) {
    return {(*points)[0]};
  }

  vector<Point> lower = buildLowerHull(points);
  vector<Point> upper = buildUpperHull(points);

  lower.pop_back();
  upper.pop_back();

  lower.insert(lower.end(), upper.begin(), upper.end());
  return lower;
}

double polygonPerimeter(const vector<Point> &poly) {
  if (poly.size() <= 1) {
    return 0.0;
  }

  double perimeter = 0.0;
  for (size_t i = 0; i < poly.size(); ++i) {
    perimeter += dist(poly[i], poly[(i + 1) % poly.size()]);
  }
  return perimeter;
}

int main() {
  int N;
  cin >> N;
  vector<Point> points(N);
  for (int i = 0; i < N; i++) {
    cin >> points[i].x >> points[i].y;
  }

  sort(points.begin(), points.end(), [](const Point &a, const Point &b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
  });

  points.erase(unique(points.begin(), points.end(), samePoint), points.end());

  vector<Point> hull = buildConvexHull(&points);
  cout << fixed << setprecision(10) << polygonPerimeter(hull) << '\n';

  return 0;
}
