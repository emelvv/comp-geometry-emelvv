#include <bits/stdc++.h>
using namespace std;

const int MAX_RECTANGLES = 10000;

struct Rectangle {
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
};

struct Query {
    int x = 0;
    int y = 0;
};

struct Event {
    int x = 0;
    int type = 0;
    int id = 0;
    int left = 0;
    int right = 0;
    int query_index = 0;
    int y_index = 0;
};

bool operator<(const Event& first, const Event& second) {
    if (first.x != second.x) {
        return first.x < second.x;
    }
    return first.type < second.type;
}

struct SegmentTree {
    int size = 0;
    vector<bitset<MAX_RECTANGLES + 1>> tree;

    explicit SegmentTree(int size) : size(size), tree(4 * max(1, size)) {}

    void update(int vertex, int tree_left, int tree_right, int query_left,
                int query_right, int id, bool is_added) {
        if (query_left > query_right) {
            return;
        }

        if (query_left == tree_left && query_right == tree_right) {
            if (is_added) {
                tree[vertex].set(id);
            } else {
                tree[vertex].reset(id);
            }
            return;
        }

        int middle = (tree_left + tree_right) / 2;
        update(2 * vertex, tree_left, middle, query_left,
               min(query_right, middle), id, is_added);
        update(2 * vertex + 1, middle + 1, tree_right,
               max(query_left, middle + 1), query_right, id, is_added);
    }

    void update(int left, int right, int id, bool is_added) {
        update(1, 0, size - 1, left, right, id, is_added);
    }

    void collect(int vertex, int tree_left, int tree_right, int position,
                 bitset<MAX_RECTANGLES + 1>& result) const {
        result |= tree[vertex];

        if (tree_left == tree_right) {
            return;
        }

        int middle = (tree_left + tree_right) / 2;
        if (position <= middle) {
            collect(2 * vertex, tree_left, middle, position, result);
        } else {
            collect(2 * vertex + 1, middle + 1, tree_right, position, result);
        }
    }

    bitset<MAX_RECTANGLES + 1> collect(int position) const {
        bitset<MAX_RECTANGLES + 1> result;
        collect(1, 0, size - 1, position, result);
        return result;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n = 0;
    cin >> n;

    vector<Rectangle> rectangles(n + 1);
    for (int id = 1; id <= n; ++id) {
        cin >> rectangles[id].x1 >> rectangles[id].y1
            >> rectangles[id].x2 >> rectangles[id].y2;
    }

    int q = 0;
    cin >> q;

    vector<Query> queries(q);
    vector<int> y_coordinates;
    y_coordinates.reserve(q);

    for (int index = 0; index < q; ++index) {
        cin >> queries[index].x >> queries[index].y;
        y_coordinates.push_back(queries[index].y);
    }

    sort(y_coordinates.begin(), y_coordinates.end());
    y_coordinates.erase(unique(y_coordinates.begin(), y_coordinates.end()),
                        y_coordinates.end());

    vector<Event> events;
    events.reserve(2 * n + q);

    for (int id = 1; id <= n; ++id) {
        int left = lower_bound(y_coordinates.begin(), y_coordinates.end(),
                               rectangles[id].y1) -
                   y_coordinates.begin();
        int right = upper_bound(y_coordinates.begin(), y_coordinates.end(),
                                rectangles[id].y2) -
                    y_coordinates.begin() - 1;

        if (left <= right) {
            events.push_back({rectangles[id].x1, 0, id, left, right, -1, -1});
            events.push_back({rectangles[id].x2, 2, id, left, right, -1, -1});
        }
    }

    for (int index = 0; index < q; ++index) {
        int y_index = lower_bound(y_coordinates.begin(), y_coordinates.end(),
                                  queries[index].y) -
                      y_coordinates.begin();
        events.push_back({queries[index].x, 1, 0, -1, -1, index, y_index});
    }

    sort(events.begin(), events.end());

    SegmentTree segment_tree(static_cast<int>(y_coordinates.size()));
    vector<bitset<MAX_RECTANGLES + 1>> answers(q);

    for (const Event& event : events) {
        if (event.type == 0) {
            segment_tree.update(event.left, event.right, event.id, true);
        } else if (event.type == 1) {
            answers[event.query_index] = segment_tree.collect(event.y_index);
        } else {
            segment_tree.update(event.left, event.right, event.id, false);
        }
    }

    for (const auto& answer : answers) {
        cout << answer.count();
        for (int id = 1; id <= n; ++id) {
            if (answer.test(id)) {
                cout << ' ' << id;
            }
        }
        cout << '\n';
    }

    return 0;
}
