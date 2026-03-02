#include <bits/stdc++.h>
using namespace std;

struct Point {
    int64_t x, y, z;

    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct Face {
    int a, b, c;
};

static int64_t dot(int64_t ax, int64_t ay, int64_t az,
                       int64_t bx, int64_t by, int64_t bz) {
    return (int64_t)ax * bx + (int64_t)ay * by + (int64_t)az * bz;
}

static int64_t volume6(const Point& a, const Point& b, const Point& c, const Point& d) {
    int64_t abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    int64_t acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
    int64_t adx = d.x - a.x, ady = d.y - a.y, adz = d.z - a.z;

    int64_t cx = aby * acz - abz * acy;
    int64_t cy = abz * acx - abx * acz;
    int64_t cz = abx * acy - aby * acx;
    return dot(cx, cy, cz, adx, ady, adz);
}

static int64_t volume6_inside4(const Point& a, const Point& b, const Point& c,
                                int64_t inside4x, int64_t inside4y, int64_t inside4z) {
    int64_t abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    int64_t acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
    int64_t vdx = inside4x - 4LL * a.x;
    int64_t vdy = inside4y - 4LL * a.y;
    int64_t vdz = inside4z - 4LL * a.z;

    int64_t cx = aby * acz - abz * acy;
    int64_t cy = abz * acx - abx * acz;
    int64_t cz = abx * acy - aby * acx;
    return dot(cx, cy, cz, vdx, vdy, vdz);
}

static inline int64_t edge_key(int u, int v) {
    return (static_cast<int64_t>(u) << 32) ^ static_cast<unsigned int>(v);
}

int main() {
    int T;
    cin >> T;
    while (T--) {
        int n;
        cin >> n;
        vector<Point> p(n);
        for (int i = 0; i < n; ++i) {
            cin >> p[i].x >> p[i].y >> p[i].z;
        }

        if (n < 4) {
            cout << 0 << '\n';
            continue;
        }

        vector<int> ord;
        ord.reserve(n);
        for (int i = 0; i < n; ++i) ord.push_back(i);

        int i0 = ord[0];
        int pos1 = -1, pos2 = -1, pos3 = -1;

        for (int i = 1; i < n; ++i) {
            if (p[ord[i]].x != p[i0].x || p[ord[i]].y != p[i0].y || p[ord[i]].z != p[i0].z) {
                pos1 = i;
                break;
            }
        }

        for (int i = 1; i < n; ++i) {
            if (i == pos1) continue;
            int64_t ux = p[ord[pos1]].x - p[i0].x;
            int64_t uy = p[ord[pos1]].y - p[i0].y;
            int64_t uz = p[ord[pos1]].z - p[i0].z;
            int64_t vx = p[ord[i]].x - p[i0].x;
            int64_t vy = p[ord[i]].y - p[i0].y;
            int64_t vz = p[ord[i]].z - p[i0].z;
            int64_t cx = uy * vz - uz * vy;
            int64_t cy = uz * vx - ux * vz;
            int64_t cz = ux * vy - uy * vx;
            if (cx != 0 || cy != 0 || cz != 0) {
                pos2 = i;
                break;
            }
        }

        for (int i = 1; i < n; ++i) {
            if (i == pos1 || i == pos2) continue;
            if (volume6(p[i0], p[ord[pos1]], p[ord[pos2]], p[ord[i]]) != 0) {
                pos3 = i;
                break;
            }
        }

        if (pos1 == -1 || pos2 == -1 || pos3 == -1) {
            cout << 0 << '\n';
            continue;
        }

        int v0 = ord[0], v1 = ord[pos1], v2 = ord[pos2], v3 = ord[pos3];
        vector<char> used(n, 0);
        used[v0] = used[v1] = used[v2] = used[v3] = 1;
        vector<int> new_ord;
        new_ord.reserve(n);
        new_ord.push_back(v0);
        new_ord.push_back(v1);
        new_ord.push_back(v2);
        new_ord.push_back(v3);
        for (int i = 0; i < n; ++i) {
            if (!used[i]) new_ord.push_back(i);
        }
        ord.swap(new_ord);

        int64_t inside4x = p[ord[0]].x + p[ord[1]].x + p[ord[2]].x + p[ord[3]].x;
        int64_t inside4y = p[ord[0]].y + p[ord[1]].y + p[ord[2]].y + p[ord[3]].y;
        int64_t inside4z = p[ord[0]].z + p[ord[1]].z + p[ord[2]].z + p[ord[3]].z;

        vector<Face> faces;
        faces.reserve(4 * n);

        auto add_face = [&](vector<Face>& target, int a, int b, int c) {
            int64_t s = volume6_inside4(p[a], p[b], p[c], inside4x, inside4y, inside4z);
            if (s > 0) swap(b, c);
            target.push_back({a, b, c});
        };

        int a = ord[0], b = ord[1], c = ord[2], d = ord[3];
        add_face(faces, a, b, c);
        add_face(faces, a, c, d);
        add_face(faces, a, d, b);
        add_face(faces, b, d, c);

        for (int it = 4; it < n; ++it) {
            int v = ord[it];
            unordered_map<int64_t, char> edge;
            edge.reserve(faces.size() * 3 + 8);
            vector<Face> kept;
            kept.reserve(faces.size() + 8);

            auto add_edge = [&](int u, int w) {
                int64_t rev = edge_key(w, u);
                auto it_rev = edge.find(rev);
                if (it_rev != edge.end()) {
                    edge.erase(it_rev);
                } else {
                    edge[edge_key(u, w)] = 1;
                }
            };

            for (const Face& f : faces) {
                if (volume6(p[f.a], p[f.b], p[f.c], p[v]) > 0) {
                    add_edge(f.a, f.b);
                    add_edge(f.b, f.c);
                    add_edge(f.c, f.a);
                } else {
                    kept.push_back(f);
                }
            }

            if (edge.empty()) {
                continue;
            }

            for (const auto& kv : edge) {
                int64_t key = kv.first;
                int u = static_cast<int>(key >> 32);
                int w = static_cast<int>(static_cast<unsigned int>(key));
                add_face(kept, u, w, v);
            }

            faces.swap(kept);
        }

        vector<Point> ans;
        ans.reserve(faces.size());
        for (const Face& f : faces) {
            int x = f.a, y = f.b, z = f.c;
            if (y < x && y <= z) {
                ans.push_back({y, z, x});
            } else if (z < x && z < y) {
                ans.push_back({z, x, y});
            } else {
                ans.push_back({x, y, z});
            }
        }

        sort(ans.begin(), ans.end());

        cout << ans.size() << '\n';
        for (const auto& t : ans) {
            cout << 3 << ' ' << t.x << ' ' << t.y << ' ' << t.z << '\n';
        }
    }

    return 0;
}
