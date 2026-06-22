#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>

using namespace std;

struct Point {
    double x, y;
    int idx;
};

struct Triangle {
    int a, b, c;
};

bool inCircumcircle(const Point& p,
                    const Point& a, const Point& b, const Point& c) {
    double ax = a.x - p.x, ay = a.y - p.y;
    double bx = b.x - p.x, by = b.y - p.y;
    double cx = c.x - p.x, cy = c.y - p.y;
    double det = ax * (by * (cx * cx + cy * cy) - cy * (bx * bx + by * by))
               - ay * (bx * (cx * cx + cy * cy) - cx * (bx * bx + by * by))
               + (ax * ax + ay * ay) * (bx * cy - by * cx);
    return det > 0.0;
}

vector<Triangle> delaunay(vector<Point>& pts, int n) {
    double minX = pts[0].x, maxX = pts[0].x;
    double minY = pts[0].y, maxY = pts[0].y;
    for (int i = 1; i < n; i++) {
        minX = min(minX, pts[i].x);
        maxX = max(maxX, pts[i].x);
        minY = min(minY, pts[i].y);
        maxY = max(maxY, pts[i].y);
    }
    double dx = maxX - minX, dy = maxY - minY;
    double delta = max(dx, dy) * 10.0;

    pts.push_back({minX - delta,     minY - delta * 3, -1});
    pts.push_back({minX + delta * 3, minY - delta,     -1});
    pts.push_back({minX - delta,     maxY + delta * 3, -1});
    int sA = n, sB = n + 1, sC = n + 2;

    vector<Triangle> tris;
    tris.push_back({sA, sB, sC});

    for (int i = 0; i < n; i++) {
        const Point& p = pts[i];

        vector<Triangle> bad;
        vector<Triangle> good;
        for (auto& t : tris) {
            if (inCircumcircle(p, pts[t.a], pts[t.b], pts[t.c]))
                bad.push_back(t);
            else
                good.push_back(t);
        }

        vector<pair<int,int>> boundary;
        for (auto& t : bad) {
            int edges[3][2] = {{t.a,t.b},{t.b,t.c},{t.c,t.a}};
            for (auto& e : edges) {
                int u = e[0], v = e[1];
                bool shared = false;
                for (auto& t2 : bad) {
                    if (&t2 == &t) continue;
                    int e2[3][2] = {{t2.a,t2.b},{t2.b,t2.c},{t2.c,t2.a}};
                    for (auto& f : e2) {
                        if ((f[0]==u && f[1]==v) || (f[0]==v && f[1]==u)) {
                            shared = true;
                            break;
                        }
                    }
                    if (shared) break;
                }
                if (!shared) boundary.push_back({u, v});
            }
        }

        tris = good;
        for (auto& e : boundary) {
            tris.push_back({e.first, e.second, i});
        }
    }

    vector<Triangle> result;
    for (auto& t : tris) {
        if (t.a < n && t.b < n && t.c < n)
            result.push_back(t);
    }
    return result;
}

bool isRNGEdge(int i, int j, const vector<Point>& pts, int n) {
    double ax = pts[i].x, ay = pts[i].y;
    double bx = pts[j].x, by = pts[j].y;
    for (int k = 0; k < n; k++) {
        if (k == i || k == j) continue;
        double cx = pts[k].x, cy = pts[k].y;
        double dot = (ax - cx) * (bx - cx) + (ay - cy) * (by - cy);
        if (dot <= 0.0) return false;
    }
    return true;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n;

    vector<Point> pts(n);
    for (int i = 0; i < n; i++) {
        cin >> pts[i].x >> pts[i].y;
        pts[i].idx = i + 1;
    }

    vector<Triangle> tris = delaunay(pts, n);

    set<pair<int,int>> edgeSet;
    for (auto& t : tris) {
        int vs[3] = {t.a, t.b, t.c};
        for (int e = 0; e < 3; e++) {
            int u = vs[e], v = vs[(e+1)%3];
            if (u > v) swap(u, v);
            edgeSet.insert({u, v});
        }
    }

    vector<pair<int,int>> result;
    for (auto& [u, v] : edgeSet) {
        if (isRNGEdge(u, v, pts, n)) {
            result.push_back({pts[u].idx, pts[v].idx});
        }
    }

    cout << result.size() << "\n";
    for (auto& [u, v] : result) {
        cout << u << " " << v << "\n";
    }

    return 0;
}
