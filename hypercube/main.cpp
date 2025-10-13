#include <vector>
#include <random>
#include <algorithm>
#include "Hypercube.h"
#include "../bruteforce/BruteForceImplementation.h"

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

    cout << endl << "Number of Vertices with points: ";
    cout << Hypercube_table.size() << endl;

    auto ids = cube_query_knn(points, points[0], /*N*/5, /*M*/100, /*probes*/12);
    cout << "Query 0: "; 
    for (auto id : ids) cout << id << " ";
    cout << endl;

    vector<double> q = points[0];
    // Brute force
    vector<pair<double,int>> all;
    all = brute_force_search(points, q, 5);

    vector<int> true_topN_ids;
    true_topN_ids.reserve(all.size());
    vector<double> true_topN_dists;
    true_topN_dists.reserve(all.size());
    for (auto& p : all) { 
        true_topN_dists.push_back(p.first);
        true_topN_ids.push_back(p.second); 
    }
    
    cout << "Query 0: "; 
    for (auto id : true_topN_ids) cout << id << " ";
    cout << endl;


    return 0;
    
}