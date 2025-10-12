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
    int n = 100;   // number of points
    int dim = 4;  // dimension of points
    double hi = 15.0;
    double low = 3.0;

    vector<vector<double>> points = generate_points(n, dim, low, hi);
    // vector<double> point = {2.3, 4.5, 3.4, 8.9};

    int k = 4;
    double w = 3.0;

    build_hypercube(points, k, w); 

    // Print
    for (auto &pair : Hypercube_table) {
        cout << pair.first << ": ";
        for (int val : pair.second) {
            cout << val << " ";
        }
        cout << endl;
    }

    cout << endl << Hypercube_table.size() << endl;
    return 0;
    
}