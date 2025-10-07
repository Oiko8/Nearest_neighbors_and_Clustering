#include <unordered_map>
#include <chrono>

#include "Euclidean_Hashing.h"

using namespace std;
typedef mt19937_64 random_generator;

/* ============================================== query function ============================================== */
void nn_query(vector<double> &q, unordered_map <int, vector<int>> table, vector<vector<double>> pts, AmplifiedHash amplified_h){
    int dim = q.size();
    
    cout << "**********************************************************" << endl;
    // Search for nearest neighor for a point
    
    cout << "Search for nearest neighor for a point" << endl;
    double min_dist = INFINITY; 
    double dist;
    vector<double> closest_point(dim);

    // counting the search time for brute force
    auto t1 = chrono::high_resolution_clock::now();

    for (vector<double> p : pts){
        dist = euclidean_distance(p, q);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = p;
        }
    }  
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms_bruteforce = std::chrono::duration<double, std::milli>(t2 - t1).count();

    cout << "Point to examine: " << "(" ;
    for (double ax : q) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    cout << "Brute-force total: " << ms_bruteforce << endl;
    cout << "**********************************************************" << endl;

    // Search for nearest neighor for a point USING THE HASH TABLE
    cout << "Search for nearest neighor for a point through the hash table" << endl;
    int bucket_of_point = amplified_h.get_amplified_id(q);
    // cout << bucket_of_point << endl;

    min_dist = INFINITY;

    // counting the search time using the hash table
    auto t3 = std::chrono::high_resolution_clock::now();

    for (int id : table[bucket_of_point]) {
        dist = euclidean_distance(q, pts[id]);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = pts[id];
        }
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    double ms_lsh = std::chrono::duration<double, std::milli>(t4 - t3).count();

    cout << "Point to examine: " << "(" ;
    for (double ax : q) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    cout << "LSH (L=1) total: " << ms_lsh << endl;

    cout << "**********************************************************" << endl;

}



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

using Table = unordered_map<int, vector<int>>;

int main() {

    /////////////////////////////////// small test ////////////////////////////////////////
    int n = 10000;     // number of points int the set 
    int dim = 3;       // dimension of the points
    double a = -5.0;   // lowest of each dimension
    double b = 15.0;   // highest of each dimension
    vector<vector<double>> pts = generate_points(n, dim, a, b);

    // cout << pts.size() << endl;
    // return 0;

    int L = 1;      // number of tables 
    double w = 3;   // window of each bucket in the table
    int khash = 3;      // hashes per amplified g function
    
    build_hash_tables(pts, L, khash, w);
    Table table = tables[0];

    // query
    vector<double> q{5.2, 2.1, 0.3};

    // test using the local function
    nn_query(q, table, pts, amplified_functions[0]);

   

    return 0;

} 








// RESULTS
/* ============================================================================================================= */
/* ============================================================================================================= */
// **********************************************************
// Search for nearest neighor for a point
// Point to examine: (5.4,2.1,0.3,)
// Closest point: (6.49141,2.45775,0.477482,)
// Distance: 1.16218
// Brute-force total: 0.021505
// **********************************************************
// Search for nearest neighor for a point through the hash table
// Point to examine: (5.4,2.1,0.3,)
// Closest point: (6.49141,2.45775,0.477482,)
// Distance: 1.16218
// LSH (L=1) total: 0.000391
// **********************************************************

/* ============================================================================================================= */
/* ============================================================================================================= */