#include <bits/stdc++.h>

using namespace std;

struct Point {
    int x = 0;
    int y = 0;
};

struct Node {
    Point point;
    int left = -1;
    int right = -1;
    int min_x = 0;
    int max_x = 0;
    int min_y = 0;
    int max_y = 0;
};

struct Candidate {
    int64_t distance = LLONG_MAX;
    Point point;
};

vector<Point> points;
vector<Node> tree;

int64_t squared_distance(const Point& first, const Point& second) {
    int64_t dx = static_cast<int64_t>(first.x) - second.x;
    int64_t dy = static_cast<int64_t>(first.y) - second.y;
    return dx * dx + dy * dy;
}

int64_t rectangle_distance(const Point& query, const Node& node) {
    int64_t dx = 0;
    if (query.x < node.min_x) {
        dx = static_cast<int64_t>(node.min_x) - query.x;
    } else if (query.x > node.max_x) {
        dx = static_cast<int64_t>(query.x) - node.max_x;
    }

    int64_t dy = 0;
    if (query.y < node.min_y) {
        dy = static_cast<int64_t>(node.min_y) - query.y;
    } else if (query.y > node.max_y) {
        dy = static_cast<int64_t>(query.y) - node.max_y;
    }

    return dx * dx + dy * dy;
}

void relax_best(const Point& point, const Point& query, Candidate best[2]) {
    int64_t distance = squared_distance(point, query);
    if (distance < best[0].distance) {
        best[1] = best[0];
        best[0] = { distance, point };
    } else if (distance < best[1].distance) {
        best[1] = { distance, point };
    }
}

int build(int left, int right) {
    if (left >= right) {
        return -1;
    }

    int min_x = points[left].x;
    int max_x = points[left].x;
    int min_y = points[left].y;
    int max_y = points[left].y;
    for (int i = left + 1; i < right; ++i) {
        min_x = min(min_x, points[i].x);
        max_x = max(max_x, points[i].x);
        min_y = min(min_y, points[i].y);
        max_y = max(max_y, points[i].y);
    }

    int axis = (max_x - min_x >= max_y - min_y ? 0 : 1);
    int middle = left + (right - left) / 2;
    nth_element(points.begin() + left, points.begin() + middle, points.begin() + right,
        [axis](const Point& first, const Point& second) {
            if (axis == 0) {
                return first.x == second.x ? first.y < second.y : first.x < second.x;
            }
            return first.y == second.y ? first.x < second.x : first.y < second.y;
        });

    int node_index = static_cast<int>(tree.size());
    tree.push_back({ points[middle], -1, -1, min_x, max_x, min_y, max_y });
    tree[node_index].left = build(left, middle);
    tree[node_index].right = build(middle + 1, right);
    return node_index;
}

void find_two_nearest(int node_index, const Point& query, Candidate best[2]) {
    if (node_index == -1) {
        return;
    }

    const Node& node = tree[node_index];
    if (rectangle_distance(query, node) > best[1].distance) {
        return;
    }

    relax_best(node.point, query, best);

    int first_child = node.left;
    int second_child = node.right;
    int64_t first_distance = first_child == -1 ? LLONG_MAX : rectangle_distance(query, tree[first_child]);
    int64_t second_distance = second_child == -1 ? LLONG_MAX : rectangle_distance(query, tree[second_child]);
    if (second_distance < first_distance) {
        swap(first_child, second_child);
        swap(first_distance, second_distance);
    }

    if (first_distance <= best[1].distance) {
        find_two_nearest(first_child, query, best);
    }
    if (second_distance <= best[1].distance) {
        find_two_nearest(second_child, query, best);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n = 0;
    cin >> n;
    points.resize(n);
    for (Point& point : points) {
        cin >> point.x >> point.y;
    }

    tree.reserve(n);
    int root = build(0, n);

    int q = 0;
    cin >> q;
    while (q--) {
        Point query;
        cin >> query.x >> query.y;
        Candidate best[2];
        find_two_nearest(root, query, best);
        cout << best[0].point.x << ' ' << best[0].point.y << ' '
             << best[1].point.x << ' ' << best[1].point.y << '\n';
    }

    return 0;
}
