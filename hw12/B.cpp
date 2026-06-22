#include <bits/stdc++.h>
using namespace std;

const long double FIELD_SIZE = 1024.0L;
const long double HIT_RADIUS = 2.0L;
const long double HIT_RADIUS2 = HIT_RADIUS * HIT_RADIUS;
const int BUCKET_SIZE = 8;
const int MAX_DEPTH = 20;

struct Creep {
    long double x = 0.0L;
    long double y = 0.0L;
    long double health = 10.0L;
};

struct QuadTree {
    struct Node {
        long double left = 0.0L;
        long double bottom = 0.0L;
        long double size = FIELD_SIZE;
        int child[4] = { -1, -1, -1, -1 };
        vector<int> indices;

        bool is_leaf() const {
            return child[0] == -1 && child[1] == -1 && child[2] == -1 && child[3] == -1;
        }
    };

    vector<Node> nodes;

    int child_index(const Creep& creep, const Node& node) const {
        long double half = node.size / 2.0L;
        long double middle_x = node.left + half;
        long double middle_y = node.bottom + half;

        int result = 0;
        if (creep.x >= middle_x) {
            result |= 1;
        }
        if (creep.y >= middle_y) {
            result |= 2;
        }

        return result;
    }

    int build_node(const vector<Creep>& creeps, const vector<int>& indices,
                   long double left, long double bottom, long double size, int depth) {
        int node_index = static_cast<int>(nodes.size());
        nodes.push_back({});
        nodes[node_index].left = left;
        nodes[node_index].bottom = bottom;
        nodes[node_index].size = size;

        if (static_cast<int>(indices.size()) <= BUCKET_SIZE || depth == MAX_DEPTH) {
            nodes[node_index].indices = indices;
            return node_index;
        }

        vector<int> child_indices[4];
        for (int index : indices) {
            child_indices[child_index(creeps[index], nodes[node_index])].push_back(index);
        }

        long double half = size / 2.0L;
        for (int child = 0; child < 4; ++child) {
            if (child_indices[child].empty()) {
                continue;
            }

            long double child_left = left + (child & 1 ? half : 0.0L);
            long double child_bottom = bottom + (child & 2 ? half : 0.0L);
            nodes[node_index].child[child] =
                build_node(creeps, child_indices[child], child_left, child_bottom, half, depth + 1);
        }

        return node_index;
    }

    void build(const vector<Creep>& creeps) {
        nodes.clear();
        nodes.reserve(creeps.size() * 4 + 1);

        vector<int> indices(creeps.size());
        iota(indices.begin(), indices.end(), 0);
        build_node(creeps, indices, 0.0L, 0.0L, FIELD_SIZE, 0);
    }

    long double min_distance_to_square2(const Creep& creep, const Node& node) const {
        long double right = node.left + node.size;
        long double top = node.bottom + node.size;
        long double dx = 0.0L;
        long double dy = 0.0L;

        if (creep.x < node.left) {
            dx = node.left - creep.x;
        } else if (creep.x > right) {
            dx = creep.x - right;
        }

        if (creep.y < node.bottom) {
            dy = node.bottom - creep.y;
        } else if (creep.y > top) {
            dy = creep.y - top;
        }

        return dx * dx + dy * dy;
    }

    void collect_near(int node_index, const Creep& creep, vector<int>& result) const {
        const Node& node = nodes[node_index];
        if (min_distance_to_square2(creep, node) >= HIT_RADIUS2) {
            return;
        }

        if (node.is_leaf()) {
            result.insert(result.end(), node.indices.begin(), node.indices.end());
            return;
        }

        for (int child : node.child) {
            if (child != -1) {
                collect_near(child, creep, result);
            }
        }
    }

    vector<int> collect_near(const Creep& creep) const {
        vector<int> result;
        if (!nodes.empty()) {
            collect_near(0, creep, result);
        }
        return result;
    }
};

long double squared_distance(const Creep& first, const Creep& second) {
    long double dx = first.x - second.x;
    long double dy = first.y - second.y;
    return dx * dx + dy * dy;
}

long double hit(const Creep& attacker, vector<Creep>& targets, const QuadTree& tree) {
    if (attacker.health <= 0.0L) {
        return 0.0L;
    }

    long double total_damage = 0.0L;
    vector<int> candidates = tree.collect_near(attacker);

    for (int target_index : candidates) {
        Creep& target = targets[target_index];
        long double distance2 = squared_distance(attacker, target);
        if (distance2 >= HIT_RADIUS2 || target.health <= 0.0L) {
            continue;
        }

        long double distance = sqrtl(distance2);
        long double factor = min(1.0L, HIT_RADIUS - distance);
        long double damage = min(target.health, attacker.health * factor / 10.0L);
        target.health -= damage;
        total_damage += damage;
    }

    return total_damage;
}

long double process_phase(vector<Creep>& active, vector<Creep>& targets, QuadTree& target_tree) {
    target_tree.build(targets);

    long double total_damage = 0.0L;
    for (Creep& creep : active) {
        char command = '\0';
        cin >> command;

        if (command == 'm') {
            long double vx = 0.0L;
            long double vy = 0.0L;
            cin >> vx >> vy;
            creep.x += vx;
            creep.y += vy;
        } else {
            total_damage += hit(creep, targets, target_tree);
        }
    }

    return total_damage;
}

int main() {
    int n = 0;
    cin >> n;

    vector<Creep> light(n);
    vector<Creep> dark(n);

    for (Creep& creep : light) {
        cin >> creep.x >> creep.y;
    }
    for (Creep& creep : dark) {
        cin >> creep.x >> creep.y;
    }

    int m = 0;
    cin >> m;

    QuadTree light_tree;
    QuadTree dark_tree;

    cout << fixed << setprecision(10);
    while (m--) {
        cout << process_phase(light, dark, dark_tree) << '\n';
        cout << process_phase(dark, light, light_tree) << '\n';
    }

    return 0;
}