// testing the parameters

#include "Euclidean_Hashing.h"
#include "../../utils_functions/Data_loader.h"
#include "../../utils_functions/euclid.h"
#include <fstream>

#define PATH "../MNIST_data/input.dat"

int main() {
    vector<vector<float>> pts;
    vector<vector<float>> queries;

    // From the input file extract the data and transform the correct way
    string input_file = PATH;
    cout << "Loading dataset: " << input_file << endl;
    pts = load_mnist_dataset(input_file);
    cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;

    cout << "*************************************************\n";

    // // From the query file extract the data and transform the correct way
    // string query_file = args.query_path;
    // cout << "Loading queries: " << query_file << endl;
    // queries = load_mnist_dataset(query_file);
    // cout << "Loaded " << queries.size() << " test images of dimension " << queries[0].size() << endl;

    int L = 1, khash = 1;
    float w = 100.0;

    int dim = pts[0].size();

    Hash h(w, dim);
    AmplifiedHash g(4, w, pts.size()/8 , dim);
    vector<int> ids;
    vector<int> amplified_ids;
    unsigned int idg;
    for (auto p:pts) {
        ids.push_back(h.get_hash_id(p));
        idg = g.get_amplified_id(p);
        amplified_ids.push_back(idg);
        // cout << idg << endl;
    }
    sort(amplified_ids.begin(), amplified_ids.end());
    for (auto id: amplified_ids) {
        cout << id << endl;
    }

    cout << ids.size() << endl;
    // auto min_h = min_element(ids.begin(), ids.end());
    // auto max_h = max_element(ids.begin(), ids.end());
    // cout << min_h[0] << endl;
    // cout << max_h[0] << endl;

    // auto min_g = min_element(amplified_ids.begin(), amplified_ids.end());
    // auto max_g = max_element(amplified_ids.begin(), amplified_ids.end());
    // cout << min_g[0] << endl;
    // cout << max_g[0] << endl;
    
    int a = -101;
    int b = 3;
    cout << (a % b + b) %b << endl;
    // // Prepare the tables according to the data
    // build_hash_tables(pts, L, khash, w);
    return 0;
}