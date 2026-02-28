#include <bits/stdc++.h>
using namespace std;

struct Point {
  int64_t x, y;
};

int64_t cross(const Point &a, const Point &b, const Point &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool onSegment(const Point &p, const Point &a, const Point &b) {
  if (cross(a, b, p) != 0) {
    return false;
  }
  return min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) && min(a.y, b.y) <= p.y &&
         p.y <= max(a.y, b.y);
}

bool inPolygon(const vector<Point> &poly, const Point &p) {
  int n = static_cast<int>(poly.size());
  int winding = 0;

  for (int i = 0; i < n; ++i) {
    const Point &a = poly[i];
    const Point &b = poly[(i + 1) % n];

    if (onSegment(p, a, b)) {
      return true;
    }

    if (a.y <= p.y) {
      if (b.y > p.y && cross(a, b, p) > 0) {
        ++winding;
      }
    } else {
      if (b.y <= p.y && cross(a, b, p) < 0) {
        --winding;
      }
    }
  }

  return winding != 0;
}

int main() {
  int n;
  Point p;
  cin >> n >> p.x >> p.y;

  vector<Point> vertices(n);
  for (int i = 0; i < n; ++i) {
    cin >> vertices[i].x >> vertices[i].y;
  }

  if (inPolygon(vertices, p)) {
    cout << "YES";
  } else {
    cout << "NO";
  }
  return 0;
}
