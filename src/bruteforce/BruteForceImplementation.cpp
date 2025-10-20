#include "BruteForceImplementation.h"
#include "../../utils_functions/Euclidean_dist.h"

vector <pair<float,int>> brute_force_search(const vector<vector<float>> &pts, vector<float> q, int N){
    // collect the distances for all the queries and get the N top
    vector<pair<float,int>> all;
    all.reserve(pts.size());
    double dist = 0.0;
    for (int i = 0 ; i < static_cast<int>(pts.size()) ; i++) {
        dist = euclidean_distance(pts[i], q);
        all.emplace_back(dist, i);
    }

    // use nth_element() and after sort only the n smaller distances
    nth_element(all.begin(), all.begin() + N, all.end(),
                [](auto& a, auto& b){ return a.first < b.first; });
    // keeping only the first N elements, that are the N smallest values
    all.resize(N);

    sort(all.begin(), all.end());

    return all;
}

