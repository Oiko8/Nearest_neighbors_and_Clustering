#include "Euclidean_Hashing.h"


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


/* =============================================================================================== */
/* ========================================= Main ================================================ */
/* =============================================================================================== */


int main() {

    /////////////////////////////////// small test ////////////////////////////////////////
    int n = 1000000;     // number of points int the set 
    int dim = 4;       // dimension of the points
    double a = -5.0;   // lowest of each dimension
    double b = 15.0;   // highest of each dimension
    vector<vector<double>> pts = generate_points(n, dim, a, b);
    // query
    vector<vector<double>> queries = generate_points(2, dim, a, b, 69);
    vector<double> q = queries[0];

    int L = 4;      // number of tables 
    double w = 3;   // window of each bucket in the table
    int khash = 3;      // hashes per amplified g function
    
    build_hash_tables(pts, L, khash, w);
    Table table = tables[0];

    /* ======================== NN SEARCH ============================= */
    int k = 5; 
    vector <int> nearest_neighbor_idx = query_knn(pts, q, k);
    cout << "**********************************************************\n";
    cout << "K="<< k << " Nearest Neighbor in each bucket:\n";
    int i = 1;
    for (int idx:nearest_neighbor_idx) {
        cout << "(";
        for (auto ax:pts[idx]) cout << ax << ", ";
        cout << ")" << endl;
        i++;
    }
    
    
    
    /* ======================= RANGE SEARCH =========================== */
    double R = 0.6;

    vector<int> pts_idx_in_range = range_search(pts, q, R);

    cout << "**********************************************************\n";
    cout << "Points in range R=" << R <<" from the query point:\n";
    for (int idx:pts_idx_in_range) {
        cout << "(";
        for (auto ax:pts[idx]) cout << ax << ", ";
        cout << ")" << endl;
    }
    cout << "**********************************************************\n";




    return 0;

} 








// RESULTS
/* ============================================================================================================= */
/* ============================================================================================================= */

// Results from 4 different hash tables
// At least in one of them(here in 3) the real nearest neighbor will be found
// **********************************************************
// oiko@OikoLenovo:~/project/project1/lsh$ ./main
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.13278,2.32067,0.412281,)
// Distance: 0.256558
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.48184,2.20735,0.200077,)
// Distance: 0.317715
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.48184,2.20735,0.200077,)
// Distance: 0.317715
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.13278,2.32067,0.412281,)
// Distance: 0.256558
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.13278,2.32067,0.412281,)
// Distance: 0.256558
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.13278,2.32067,0.412281,)
// Distance: 0.256558
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.13278,2.32067,0.412281,)
// Distance: 0.256558
// **********************************************************
// **********************************************************
// Point to examine: (5.2,2.1,0.3,)
// Closest point: (5.48184,2.20735,0.200077,)
// Distance: 0.317715
// **********************************************************
// oiko@OikoLenovo:~/project/project1/lsh$ 

/* ============================================================================================================= */
/* ============================================================================================================= */