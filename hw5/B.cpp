#include <bits/stdc++.h>
using namespace std;

constexpr long double kEps = 1e-12L;

struct Point3 {
    long double x = 0;
    long double y = 0;
    long double z = 0;
};

struct Face {
    vector<int> v = vector<int>(3);
    vector<int> adj = vector<int>(3, -1);
    bool alive = true;
    vector<int> conflict;
};

Point3 operator-(const Point3& lhs, const Point3& rhs) {
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

Point3 cross(const Point3& lhs, const Point3& rhs) {
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

long double dot(const Point3& lhs, const Point3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

long double orient(const Point3& a, const Point3& b, const Point3& c, const Point3& d) {
    return dot(cross(b - a, c - a), d - a);
}

pair<int, int> edge_vertices(const Face& face, int edge_id) {
    return { face.v[(edge_id + 1) % 3], face.v[(edge_id + 2) % 3] };
}

int find_edge(const Face& face, int a, int b) {
    for (int edge_id = 0; edge_id < 3; ++edge_id) {
        auto [u, v] = edge_vertices(face, edge_id);
        if ((u == a && v == b) || (u == b && v == a)) {
            return edge_id;
        }
    }
    return -1;
}

bool visible(const Face& face, const vector<Point3>& pts, int point_id) {
    return orient(pts[face.v[0]], pts[face.v[1]], pts[face.v[2]], pts[point_id]) > kEps;
}

Face make_face(int a, int b, int c, const vector<Point3>& pts, const Point3& inner) {
    if (orient(pts[a], pts[b], pts[c], inner) > 0) {
        swap(b, c);
    }
    Face face;
    face.v[0] = a;
    face.v[1] = b;
    face.v[2] = c;
    return face;
}

void connect_faces(vector<Face>& faces, int face_a, int edge_a, int face_b, int edge_b) {
    faces[face_a].adj[edge_a] = face_b;
    faces[face_b].adj[edge_b] = face_a;
}

uint64_t directed_key(int from, int to) {
    return (static_cast<uint64_t>(static_cast<uint32_t>(from)) << 32U) |
        static_cast<uint32_t>(to);
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<Point3> points(n);
    for (Point3& point : points) {
        double x;
        double y;
        double z;
        cin >> x >> y >> z;
        point.x = x;
        point.y = y;
        point.z = z;
    }

    mt19937 rng(712367821);
    shuffle(points.begin(), points.end(), rng);

    Point3 inner{
        (points[0].x + points[1].x + points[2].x + points[3].x) / 4.0L,
        (points[0].y + points[1].y + points[2].y + points[3].y) / 4.0L,
        (points[0].z + points[1].z + points[2].z + points[3].z) / 4.0L
    };

    vector<Face> faces;
    faces.reserve(4 * n + 16);
    faces.push_back(make_face(0, 1, 2, points, inner));
    faces.push_back(make_face(0, 3, 1, points, inner));
    faces.push_back(make_face(0, 2, 3, points, inner));
    faces.push_back(make_face(1, 3, 2, points, inner));

    {
        unordered_map<uint64_t, pair<int, int>> pending;
        pending.reserve(16);
        for (int face_id = 0; face_id < 4; ++face_id) {
            for (int edge_id = 0; edge_id < 3; ++edge_id) {
                auto [u, v] = edge_vertices(faces[face_id], edge_id);
                uint64_t reverse = directed_key(v, u);
                auto it = pending.find(reverse);
                if (it != pending.end()) {
                    connect_faces(faces, face_id, edge_id, it->second.first, it->second.second);
                    pending.erase(it);
                }
                else {
                    pending[directed_key(u, v)] = { face_id, edge_id };
                }
            }
        }
    }

    vector<int> owner(n, -1);
    for (int point_id = 4; point_id < n; ++point_id) {
        for (int face_id = 0; face_id < 4; ++face_id) {
            if (visible(faces[face_id], points, point_id)) {
                owner[point_id] = face_id;
                faces[face_id].conflict.push_back(point_id);
                break;
            }
        }
    }

    vector<int> mark(4, 0);
    int stamp = 0;

    for (int point_id = 4; point_id < n; ++point_id) {
        int start_face = owner[point_id];
        if (start_face == -1) {
            continue;
        }

        if (!faces[start_face].alive || !visible(faces[start_face], points, point_id)) {
            start_face = -1;
            for (int face_id = 0; face_id < static_cast<int>(faces.size()); ++face_id) {
                if (faces[face_id].alive && visible(faces[face_id], points, point_id)) {
                    start_face = face_id;
                    break;
                }
            }
            if (start_face == -1) {
                owner[point_id] = -1;
                continue;
            }
        }

        if (static_cast<int>(mark.size()) < static_cast<int>(faces.size())) {
            mark.resize(faces.size(), 0);
        }
        ++stamp;

        vector<int> stack{ start_face };
        vector<int> visible_faces;
        mark[start_face] = stamp;

        while (!stack.empty()) {
            int face_id = stack.back();
            stack.pop_back();
            visible_faces.push_back(face_id);

            for (int edge_id = 0; edge_id < 3; ++edge_id) {
                int next_face = faces[face_id].adj[edge_id];
                if (next_face != -1 && faces[next_face].alive && mark[next_face] != stamp &&
                    visible(faces[next_face], points, point_id)) {
                    mark[next_face] = stamp;
                    stack.push_back(next_face);
                }
            }
        }

        struct HorizonEdge {
            int u = -1;
            int v = -1;
            int neighbor = -1;
        };

        vector<HorizonEdge> horizon;
        horizon.reserve(visible_faces.size() + 3);
        vector<int> reassigned_points;

        for (int face_id : visible_faces) {
            for (int candidate : faces[face_id].conflict) {
                owner[candidate] = -1;
                reassigned_points.push_back(candidate);
            }
            faces[face_id].conflict.clear();
        }

        for (int face_id : visible_faces) {
            Face& face = faces[face_id];
            for (int edge_id = 0; edge_id < 3; ++edge_id) {
                int next_face = face.adj[edge_id];
                if (next_face == -1 || mark[next_face] != stamp) {
                    auto [u, v] = edge_vertices(face, edge_id);
                    horizon.push_back({ u, v, next_face });
                }
            }
            face.alive = false;
        }

        vector<int> new_faces;
        new_faces.reserve(horizon.size());
        for (const HorizonEdge& edge : horizon) {
            int face_id = static_cast<int>(faces.size());
            faces.push_back(make_face(edge.u, edge.v, point_id, points, inner));
            if (static_cast<int>(mark.size()) < static_cast<int>(faces.size())) {
                mark.push_back(0);
            }

            int border_edge = find_edge(faces.back(), edge.u, edge.v);
            if (edge.neighbor != -1) {
                int neighbor_edge = find_edge(faces[edge.neighbor], edge.u, edge.v);
                connect_faces(faces, face_id, border_edge, edge.neighbor, neighbor_edge);
            }
            new_faces.push_back(face_id);
        }

        unordered_map<uint64_t, pair<int, int>> pending;
        pending.reserve(new_faces.size() * 2 + 1);
        for (int face_id : new_faces) {
            for (int edge_id = 0; edge_id < 3; ++edge_id) {
                if (faces[face_id].adj[edge_id] != -1) {
                    continue;
                }
                auto [u, v] = edge_vertices(faces[face_id], edge_id);
                uint64_t reverse = directed_key(v, u);
                auto it = pending.find(reverse);
                if (it != pending.end()) {
                    connect_faces(faces, face_id, edge_id, it->second.first, it->second.second);
                    pending.erase(it);
                }
                else {
                    pending[directed_key(u, v)] = { face_id, edge_id };
                }
            }
        }

        for (int candidate : reassigned_points) {
            if (candidate <= point_id) {
                continue;
            }
            for (int face_id : new_faces) {
                if (visible(faces[face_id], points, candidate)) {
                    owner[candidate] = face_id;
                    faces[face_id].conflict.push_back(candidate);
                    break;
                }
            }
        }
    }

    long double answer = 0;
    for (const Face& face : faces) {
        if (!face.alive) {
            continue;
        }
        const Point3& a = points[face.v[0]];
        const Point3& b = points[face.v[1]];
        const Point3& c = points[face.v[2]];
        Point3 normal = cross(b - a, c - a);
        if (normal.z > 0) {
            answer += sqrt(dot(normal, normal)) / 2.0L;
        }
    }

    cout << fixed << setprecision(10) << static_cast<double>(answer) << '\n';
    return 0;
}
