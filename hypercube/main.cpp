#include <vector>
#include <random>
#include <algorithm>
#include "Hypercube.h"

using random_generator = mt19937_64;
using namespace std;

/* =================================== helper function to generate point ====================================== */
vector<vector<double>> generate_points(int n, int dim,
                                       double lo, double hi,
                                       uint64_t seed = 42) {
    random_generator gen(seed);
    uniform_real_distribution<double> dist(lo, hi);

    vector<vector<double>> pts(n, vector<double>(dim));
    for (int i = 0; i < n; ++i) {
        for (int d = 0; d < dim; ++d) {
            pts[i][d] = dist(gen);
        }
    }
    return pts;
}


int main() {
    int n = 10;   // number of points
    int dim = 4;  // dimension of points
    double hi = 15.0;
    double low = 3.0;

    vector<vector<double>> points = generate_points(n, dim, low, hi);
    // vector<double> point = {2.3, 4.5, 3.4, 8.9};

    double w = 2.0; int d = static_cast<int> (points[0].size());
    vector<Hash> hashes;
    int k = 4;
    for (int i = 0 ; i < k ; i++) {
        Hash new_h(w, d);
        hashes.push_back(new_h);
    }


    BitMapper bit_map(k);
    int p_idx = 1;
    for (auto point: points) {
        cout << "Point " << p_idx << ":" <<endl;
        for (int i = 0 ; i < k ; i++) {
            int h_id = hashes[i].get_hash_id(point);
            cout << h_id << " ---> ";
            cout << bit_map.bit_for(i, h_id) << endl;
        }
        cout << "====================================\n";
        p_idx++;
    }

    return 0;
    
}