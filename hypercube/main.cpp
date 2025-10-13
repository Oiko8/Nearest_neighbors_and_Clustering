#include <vector>
#include <random>
#include <algorithm>
#include "Hypercube.h"
#include "../bruteforce/BruteForceImplementation.h"
#include "../utils_functions/Data_loader.h"

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
    string input_file = "../MNIST_data/input.dat";
    string query_file = "../MNIST_data/query.dat";
    
    // From the input file extract the data and transform the correct way
    cout << "Loading dataset: " << input_file << endl;
    vector<vector<double>> points = load_mnist_dataset(input_file);
    cout << "Loaded " << points.size() << " images of dimension " << points.size() << endl;

    cout << "*************************************************\n";

    // From the query file extract the data and transform the correct way
    cout << "Loading queries: " << query_file << endl;
    vector<vector<double>> queries = load_mnist_dataset(query_file);
    cout << "Loaded " << queries.size() << " test images of dimension " << queries[0].size() << endl;


    // Normalize the vectors: 0-255 --> 0-1
    for (auto &point : points){
        for (double &dim : point) {
            dim /= 255.0;
        }
    }
    for (auto &query : queries){
        for (double &dim : query) {
            dim /= 255.0;
        }
    }

    int k = 14;
    double w = 8.0;

    build_hypercube(points, k, w); 


    cout << endl << "Number of Vertices with points: ";
    cout << Hypercube_table.size() << endl;
    
    vector<double> q = queries[0];
    auto ids = cube_query_knn(points, q, /*N*/5, /*M*/1000, /*probes*/100);
    cout << "Query 0: "; 
    for (auto id : ids) cout << id << " ";
    cout << endl;

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