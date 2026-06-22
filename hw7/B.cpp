#include <bits/stdc++.h>

using namespace std;

constexpr long double kEps = 1e-12L;

int sign(long double value) {
    if (value > kEps) {
        return 1;
    }
    if (value < -kEps) {
        return -1;
    }
    return 0;
}

struct Point {
    long double x = 0;
    long double y = 0;

    Point() = default;
    Point(long double x_value, long double y_value) : x(x_value), y(y_value) {
    }

    Point operator-(const Point& other) const {
        return { x - other.x, y - other.y };
    }

    long double cross(const Point& other) const {
        return x * other.y - y * other.x;
    }

    long double cross(const Point& a, const Point& b) const {
        return (a - *this).cross(b - *this);
    }

    long double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }

    long double dot(const Point& a, const Point& b) const {
        return (a - *this).dot(b - *this);
    }

    long double squared_length() const {
        return dot(*this);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

Point kInfPoint(1e30L, 1e30L);

struct QuadEdge {
    Point origin;
    QuadEdge* rot = nullptr;
    QuadEdge* onext = nullptr;
    bool used = false;

    QuadEdge* rev() const {
        return rot->rot;
    }

    QuadEdge* lnext() const {
        return rot->rev()->onext->rot;
    }

    QuadEdge* oprev() const {
        return rot->onext->rot;
    }

    Point dest() const {
        return rev()->origin;
    }
};

QuadEdge* make_edge(const Point& from, const Point& to) {
    QuadEdge* edge1 = new QuadEdge;
    QuadEdge* edge2 = new QuadEdge;
    QuadEdge* edge3 = new QuadEdge;
    QuadEdge* edge4 = new QuadEdge;

    edge1->origin = from;
    edge2->origin = to;
    edge3->origin = kInfPoint;
    edge4->origin = kInfPoint;

    edge1->rot = edge3;
    edge2->rot = edge4;
    edge3->rot = edge2;
    edge4->rot = edge1;

    edge1->onext = edge1;
    edge2->onext = edge2;
    edge3->onext = edge4;
    edge4->onext = edge3;
    return edge1;
}

void splice(QuadEdge* first, QuadEdge* second) {
    swap(first->onext->rot->onext, second->onext->rot->onext);
    swap(first->onext, second->onext);
}

void delete_edge(QuadEdge* edge) {
    splice(edge, edge->oprev());
    splice(edge->rev(), edge->rev()->oprev());
    delete edge->rev()->rot;
    delete edge->rev();
    delete edge->rot;
    delete edge;
}

QuadEdge* connect(QuadEdge* first, QuadEdge* second) {
    QuadEdge* edge = make_edge(first->dest(), second->origin);
    splice(edge, first->lnext());
    splice(edge->rev(), second);
    return edge;
}

bool left_of(const Point& point, QuadEdge* edge) {
    return sign(point.cross(edge->origin, edge->dest())) > 0;
}

bool right_of(const Point& point, QuadEdge* edge) {
    return sign(point.cross(edge->origin, edge->dest())) < 0;
}

long double det3(long double a1, long double a2, long double a3,
    long double b1, long double b2, long double b3,
    long double c1, long double c2, long double c3) {
    return a1 * (b2 * c3 - c2 * b3) - a2 * (b1 * c3 - c1 * b3) +
        a3 * (b1 * c2 - c1 * b2);
}

bool in_circle(const Point& a, const Point& b, const Point& c, const Point& d) {
    Point da = a - d;
    Point db = b - d;
    Point dc = c - d;
    long double det = det3(da.x, da.y, da.squared_length(),
        db.x, db.y, db.squared_length(),
        dc.x, dc.y, dc.squared_length());
    return sign(det) > 0;
}

pair<QuadEdge*, QuadEdge*> build_triangulation(int left, int right, vector<Point>& points) {
    if (right - left + 1 == 2) {
        QuadEdge* edge = make_edge(points[left], points[right]);
        return { edge, edge->rev() };
    }

    if (right - left + 1 == 3) {
        QuadEdge* first = make_edge(points[left], points[left + 1]);
        QuadEdge* second = make_edge(points[left + 1], points[right]);
        splice(first->rev(), second);

        int orientation = sign(points[left].cross(points[left + 1], points[right]));
        if (orientation == 0) {
            return { first, second->rev() };
        }

        QuadEdge* third = connect(second, first);
        if (orientation > 0) {
            return { first, second->rev() };
        }
        return { third->rev(), third };
    }

    int middle = (left + right) / 2;
    QuadEdge* left_outer = nullptr;
    QuadEdge* left_inner = nullptr;
    QuadEdge* right_inner = nullptr;
    QuadEdge* right_outer = nullptr;
    tie(left_outer, left_inner) = build_triangulation(left, middle, points);
    tie(right_inner, right_outer) = build_triangulation(middle + 1, right, points);

    while (true) {
        if (left_of(right_inner->origin, left_inner)) {
            left_inner = left_inner->lnext();
            continue;
        }
        if (right_of(left_inner->origin, right_inner)) {
            right_inner = right_inner->rev()->onext;
            continue;
        }
        break;
    }

    QuadEdge* base = connect(right_inner->rev(), left_inner);
    if (left_inner->origin == left_outer->origin) {
        left_outer = base->rev();
    }
    if (right_inner->origin == right_outer->origin) {
        right_outer = base;
    }

    auto valid = [&base](QuadEdge* edge) {
        return right_of(edge->dest(), base);
        };

    while (true) {
        QuadEdge* left_candidate = base->rev()->onext;
        if (valid(left_candidate)) {
            while (in_circle(base->dest(), base->origin,
                left_candidate->dest(), left_candidate->onext->dest())) {
                QuadEdge* next = left_candidate->onext;
                delete_edge(left_candidate);
                left_candidate = next;
            }
        }

        QuadEdge* right_candidate = base->oprev();
        if (valid(right_candidate)) {
            while (in_circle(base->dest(), base->origin,
                right_candidate->dest(), right_candidate->oprev()->dest())) {
                QuadEdge* next = right_candidate->oprev();
                delete_edge(right_candidate);
                right_candidate = next;
            }
        }

        bool left_valid = valid(left_candidate);
        bool right_valid = valid(right_candidate);
        if (!left_valid && !right_valid) {
            break;
        }

        if (!left_valid ||
            (right_valid && in_circle(left_candidate->dest(), left_candidate->origin,
                right_candidate->origin, right_candidate->dest()))) {
            base = connect(right_candidate, base->rev());
        }
        else {
            base = connect(base->rev(), left_candidate->rev());
        }
    }

    return { left_outer, right_outer };
}

vector<vector<Point>> delaunay_faces(vector<Point> points) {
    if (points.size() < 3) {
        return {};
    }

    sort(points.begin(), points.end(), [](const Point& lhs, const Point& rhs) {
        if (lhs.x != rhs.x) {
            return lhs.x < rhs.x;
        }
        return lhs.y < rhs.y;
        });

    points.erase(unique(points.begin(), points.end(), [](const Point& lhs, const Point& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
        }), points.end());

    if (points.size() < 3) {
        return {};
    }

    auto result = build_triangulation(0, static_cast<int>(points.size()) - 1, points);
    QuadEdge* edge = result.first;
    vector<QuadEdge*> stack = { edge };
    vector<vector<Point>> faces;

    while (sign(edge->onext->dest().cross(edge->dest(), edge->origin)) < 0) {
        edge = edge->onext;
    }

    auto add_face = [&faces, &stack, &edge]() {
        QuadEdge* current = edge;
        vector<Point> vertices;
        do {
            current->used = true;
            vertices.push_back(current->origin);
            stack.push_back(current->rev());
            current = current->lnext();
        } while (current != edge);

        if (vertices.size() < 3) {
            return;
        }

        long double signed_area2 = 0;
        for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
            signed_area2 += vertices[i].cross(vertices[(i + 1) % static_cast<int>(vertices.size())]);
        }
        if (sign(signed_area2) <= 0) {
            return;
        }

        faces.push_back(vertices);
    };

    add_face();
    int head = 0;
    while (head < static_cast<int>(stack.size())) {
        edge = stack[head++];
        if (!edge->used) {
            add_face();
        }
    }

    return faces;
}

long double distance_between(const Point& first, const Point& second) {
    long double dx = first.x - second.x;
    long double dy = first.y - second.y;
    return sqrtl(dx * dx + dy * dy);
}

long double circumradius(const Point& a, const Point& b, const Point& c) {
    long double area2 = fabsl(a.cross(b, c));
    if (area2 <= kEps) {
        return -1;
    }

    long double side_ab = distance_between(a, b);
    long double side_bc = distance_between(b, c);
    long double side_ca = distance_between(c, a);
    return side_ab * side_bc * side_ca / (2.0L * area2);
}

long double face_radius(const vector<Point>& face) {
    if (face.size() < 3) {
        return -1;
    }

    for (int i = 1; i + 1 < static_cast<int>(face.size()); ++i) {
        long double radius = circumradius(face[0], face[i], face[i + 1]);
        if (radius >= 0) {
            return radius;
        }
    }
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<Point> points(n);
    for (Point& point : points) {
        long double x = 0;
        long double y = 0;
        cin >> x >> y;
        point = Point(x, y);
    }

    vector<vector<Point>> faces = delaunay_faces(points);
    long double answer = 0;
    for (const auto& face : faces) {
        answer = max(answer, face_radius(face));
    }

    cout << fixed << setprecision(6) << answer << '\n';
    return 0;
}
