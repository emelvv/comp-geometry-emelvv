#include <bits/stdc++.h>
using namespace std;

struct Point {
    int64_t x, y, z;
    bool operator<(const Point& rhs) const {
        if (x != rhs.x) return x < rhs.x;
        if (y != rhs.y) return y < rhs.y;
        return z < rhs.z;
    }
};

struct Vertex {
    int pid;
};

struct HalfEdge {
    int origin;
    int twin;
    int next;
    int prev;
    int face;
    bool alive;
};

struct Face {
    int edge;
    bool alive;
};

static inline int64_t edge_key(int u, int v) {
    return (static_cast<int64_t>(u) << 32) ^ static_cast<unsigned int>(v);
}

static int64_t dot(int64_t ax, int64_t ay, int64_t az,
    int64_t bx, int64_t by, int64_t bz) {
    return (int64_t)ax * bx + (int64_t)ay * by + (int64_t)az * bz;
}

static int64_t volume(const Point& a, const Point& b, const Point& c, const Point& d) {
    int64_t abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    int64_t acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
    int64_t adx = d.x - a.x, ady = d.y - a.y, adz = d.z - a.z;

    int64_t nx = aby * acz - abz * acy;
    int64_t ny = abz * acx - abx * acz;
    int64_t nz = abx * acy - aby * acx;
    return dot(nx, ny, nz, adx, ady, adz);
}

static int64_t volume_inside4(const Point& a, const Point& b, const Point& c,
    int64_t inside4x, int64_t inside4y, int64_t inside4z) {
    int64_t abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    int64_t acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
    int64_t idx = inside4x - 4LL * a.x;
    int64_t idy = inside4y - 4LL * a.y;
    int64_t idz = inside4z - 4LL * a.z;

    int64_t nx = aby * acz - abz * acy;
    int64_t ny = abz * acx - abx * acz;
    int64_t nz = abx * acy - aby * acx;
    return dot(nx, ny, nz, idx, idy, idz);
}

struct DCEL {
    vector<Vertex> vertices;
    vector<HalfEdge> edges;
    vector<Face> faces;
    vector<int> active_faces;
    unordered_map<int64_t, int> edge_map;

    DCEL(int n = 0) {
        vertices.resize(n);
        for (int i = 0; i < n; ++i) vertices[i].pid = i;
        edge_map.reserve(1 << 14);
    }

    int add_face_raw(int a, int b, int c) {
        int f = (int)faces.size();
        int e0 = (int)edges.size();
        int e1 = e0 + 1;
        int e2 = e0 + 2;

        faces.push_back({ e0, true });

        edges.push_back({ a, -1, e1, e2, f, true });
        edges.push_back({ b, -1, e2, e0, f, true });
        edges.push_back({ c, -1, e0, e1, f, true });

        auto link_twin = [&](int eid, int u, int v) {
            int64_t rev = edge_key(v, u);
            auto it = edge_map.find(rev);
            if (it != edge_map.end()) {
                int tw = it->second;
                edges[eid].twin = tw;
                edges[tw].twin = eid;
            }
            edge_map[edge_key(u, v)] = eid;
            };

        link_twin(e0, a, b);
        link_twin(e1, b, c);
        link_twin(e2, c, a);

        active_faces.push_back(f);
        return f;
    }

    int add_face_oriented(int a, int b, int c, const vector<Point>& p,
        int64_t inside4x, int64_t inside4y, int64_t inside4z) {
        if (volume_inside4(p[a], p[b], p[c], inside4x, inside4y, inside4z) > 0) {
            swap(b, c);
        }
        return add_face_raw(a, b, c);
    }

    void remove_face(int f) {
        if (!faces[f].alive) return;
        faces[f].alive = false;
        int e0 = faces[f].edge;
        int e1 = edges[e0].next;
        int e2 = edges[e1].next;
        int arr[3] = { e0, e1, e2 };

        for (int id : arr) {
            if (!edges[id].alive) continue;
            int u = edges[id].origin;
            int v = edges[edges[id].next].origin;

            auto it = edge_map.find(edge_key(u, v));
            if (it != edge_map.end() && it->second == id) edge_map.erase(it);

            int tw = edges[id].twin;
            if (tw != -1 && edges[tw].alive) edges[tw].twin = -1;
            edges[id].alive = false;
            edges[id].twin = -1;
        }
    }
};

int main() {
    int T;
    cin >> T;

    while (T--) {
        int n;
        cin >> n;
        vector<Point> p(n);
        for (int i = 0; i < n; ++i) cin >> p[i].x >> p[i].y >> p[i].z;

        if (n < 4) {
            cout << 0 << '\n';
            continue;
        }

        int i0 = 0, i1 = -1, i2 = -1, i3 = -1;

        for (int i = 1; i < n; ++i) {
            if (p[i].x != p[i0].x || p[i].y != p[i0].y || p[i].z != p[i0].z) {
                i1 = i;
                break;
            }
        }
        if (i1 == -1) {
            cout << 0 << '\n';
            continue;
        }

        for (int i = 1; i < n; ++i) {
            if (i == i1) { continue; }
            int64_t ux = p[i1].x - p[i0].x;
            int64_t uy = p[i1].y - p[i0].y;
            int64_t uz = p[i1].z - p[i0].z;
            int64_t vx = p[i].x - p[i0].x;
            int64_t vy = p[i].y - p[i0].y;
            int64_t vz = p[i].z - p[i0].z;
            int64_t cx = uy * vz - uz * vy;
            int64_t cy = uz * vx - ux * vz;
            int64_t cz = ux * vy - uy * vx;
            if (cx != 0 || cy != 0 || cz != 0) {
                i2 = i;
                break;
            }
        }
        if (i2 == -1) {
            cout << 0 << '\n';
            continue;
        }

        for (int i = 1; i < n; ++i) {
            if (i == i1 || i == i2) { continue; }
            if (volume(p[i0], p[i1], p[i2], p[i]) != 0) {
                i3 = i;
                break;
            }
        }
        if (i3 == -1) {
            cout << 0 << '\n';
            continue;
        }

        vector<int> order;
        order.reserve(n);
        vector<char> used(n, 0);
        order.push_back(i0);
        order.push_back(i1);
        order.push_back(i2);
        order.push_back(i3);
        used[i0] = used[i1] = used[i2] = used[i3] = 1;
        for (int i = 0; i < n; ++i) if (!used[i]) order.push_back(i);

        int64_t inside4x = p[i0].x + p[i1].x + p[i2].x + p[i3].x;
        int64_t inside4y = p[i0].y + p[i1].y + p[i2].y + p[i3].y;
        int64_t inside4z = p[i0].z + p[i1].z + p[i2].z + p[i3].z;

        DCEL hull(n);
        hull.add_face_oriented(i0, i1, i2, p, inside4x, inside4y, inside4z);
        hull.add_face_oriented(i0, i2, i3, p, inside4x, inside4y, inside4z);
        hull.add_face_oriented(i0, i3, i1, p, inside4x, inside4y, inside4z);
        hull.add_face_oriented(i1, i3, i2, p, inside4x, inside4y, inside4z);

        vector<char> visible;
        visible.reserve(4 * n + 16);

        for (int pos = 4; pos < n; ++pos) {
            int v = order[pos];
            if ((int)visible.size() < (int)hull.faces.size()) visible.resize(hull.faces.size(), 0);

            vector<int> vis_faces;
            vis_faces.reserve(hull.active_faces.size());

            for (int f : hull.active_faces) {
                if (!hull.faces[f].alive) continue;
                int e0 = hull.faces[f].edge;
                int e1 = hull.edges[e0].next;
                int e2 = hull.edges[e1].next;
                int a = hull.edges[e0].origin;
                int b = hull.edges[e1].origin;
                int c = hull.edges[e2].origin;
                if (volume(p[a], p[b], p[c], p[v]) > 0) {
                    visible[f] = 1;
                    vis_faces.push_back(f);
                }
                else {
                    visible[f] = 0;
                }
            }

            if (vis_faces.empty()) continue;

            vector<pair<int, int>> horizon;
            horizon.reserve(vis_faces.size() * 3);

            for (int f : vis_faces) {
                int e0 = hull.faces[f].edge;
                int cur = e0;
                for (int rep = 0; rep < 3; ++rep) {
                    int tw = hull.edges[cur].twin;
                    bool twin_vis = false;
                    if (tw != -1) {
                        int tf = hull.edges[tw].face;
                        twin_vis = (tf < (int)visible.size() && hull.faces[tf].alive && visible[tf]);
                    }
                    if (!twin_vis) {
                        int u = hull.edges[cur].origin;
                        int w = hull.edges[hull.edges[cur].next].origin;
                        horizon.push_back({ u, w });
                    }
                    cur = hull.edges[cur].next;
                }
            }

            for (int f : vis_faces) hull.remove_face(f);

            vector<int> next_active;
            next_active.reserve(hull.active_faces.size() + horizon.size());
            for (int f : hull.active_faces) {
                if (hull.faces[f].alive) next_active.push_back(f);
            }
            hull.active_faces.swap(next_active);

            for (const auto& e : horizon) {
                hull.add_face_raw(e.first, e.second, v);
            }
        }

        vector<Point> out;
        out.reserve(hull.active_faces.size());

        for (int f : hull.active_faces) {
            if (!hull.faces[f].alive) continue;
            int e0 = hull.faces[f].edge;
            int e1 = hull.edges[e0].next;
            int e2 = hull.edges[e1].next;

            int a = hull.edges[e0].origin;
            int b = hull.edges[e1].origin;
            int c = hull.edges[e2].origin;

            if (b < a && b <= c) out.push_back({ b, c, a });
            else if (c < a && c < b) out.push_back({ c, a, b });
            else out.push_back({ a, b, c });
        }

        sort(out.begin(), out.end());
        cout << out.size() << '\n';
        for (const auto& t : out) {
            cout << 3 << ' ' << t.x << ' ' << t.y << ' ' << t.z << '\n';
        }
    }

    return 0;
}
