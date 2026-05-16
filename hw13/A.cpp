#include <bits/stdc++.h>

using namespace std;

const string BASE32 = "0123456789bcdefghjkmnpqrstuvwxyz";

int main() {
    int t = 0;
    cin >> t;

    while (t--) {
        int depth = 0;
        long double lon = 0.0L;
        long double lat = 0.0L;
        cin >> depth >> lon >> lat;

        long double left_lon = -180.0L;
        long double right_lon = 180.0L;
        long double bottom_lat = -90.0L;
        long double top_lat = 90.0L;

        string answer;
        answer.reserve(depth);

        int value = 0;
        for (int bit_number = 0; bit_number < 5 * depth; ++bit_number) {
            value <<= 1;

            if (bit_number % 2 == 0) {
                long double middle = (left_lon + right_lon) / 2.0L;
                if (lon >= middle) {
                    value |= 1;
                    left_lon = middle;
                } else {
                    right_lon = middle;
                }
            } else {
                long double middle = (bottom_lat + top_lat) / 2.0L;
                if (lat >= middle) {
                    value |= 1;
                    bottom_lat = middle;
                } else {
                    top_lat = middle;
                }
            }

            if (bit_number % 5 == 4) {
                answer.push_back(BASE32[value]);
                value = 0;
            }
        }

        cout << answer << '\n';
    }

    return 0;
}
