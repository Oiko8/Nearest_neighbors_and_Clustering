#include <unordered_map>
#include <chrono>

#include "Euclidean_Hashing.h"

using namespace std;
typedef mt19937_64 random_generator;


/* =============================== helper function to calculate euclidean distance ============================ */
double euclidean_distance(const vector<double> &p1, const vector<double> &p2){
    double sum = 0.0;
    int size = p1.size();
    for (int i = 0 ; i < size ; i++){
        sum += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return sqrt(sum);
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

int main() {
    static random_generator generator(42);

   
    /////////////////////////////////// small test ////////////////////////////////////////
    int n = 1000;
    int dim = 3;
    double a = -5.0;
    double b = 15.0;
    vector<vector<double>> pts = generate_points(n, dim, a, b);

    // cout << pts.size() << endl;
    // return 0;

    double w = 3;
    int k = 3;
    int tableSize = pts.size()/2;
    AmplifiedHash amplified_h(k, w, tableSize, generator, 2);
    unordered_map <int, vector<int>> table;

    for (int i = 0; i < (int)pts.size(); ++i) {
        table[amplified_h.get_amplified_id(pts[i])].push_back(i);
    }

    // // using auto to let the compiler decides the type
    // for (const auto& [bucket, ids] : table) {
    //     cout << "bucket " << bucket << " ->";
    //     for (int id : ids) {
    //         cout << "  " ;
    //         cout << "(" << pts[id][0] << "," << pts[id][1] << ")";
    //     }
    //     cout << "\n";
    // }

    vector<double> point{5.4, 2.1, 0.3};



     cout << "**********************************************************" << endl;
    // Search for nearest neighor for a point
    
    cout << "Search for nearest neighor for a point" << endl;
    double min_dist = INFINITY; 
    double dist;
    vector<double> closest_point(dim);

    // counting the search time for brute force
    auto t1 = chrono::high_resolution_clock::now();

    for (vector<double> p : pts){
        dist = euclidean_distance(p, point);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = p;
        }
    }  
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms_bruteforce = std::chrono::duration<double, std::milli>(t2 - t1).count();

    cout << "Point to examine: " << "(" ;
    for (double ax : point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    std::cout << "Brute-force total: " << ms_bruteforce << endl;
    cout << "**********************************************************" << endl;




    // Search for nearest neighor for a point USING THE HASH TABLE
    cout << "Search for nearest neighor for a point through the hash table" << endl;
    int bucket_of_point = amplified_h.get_amplified_id(point);
    // cout << bucket_of_point << endl;

    min_dist = INFINITY;

    // counting the search time using the hash table
    auto t3 = std::chrono::high_resolution_clock::now();

    for (int id : table[bucket_of_point]) {
        dist = euclidean_distance(point, pts[id]);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = pts[id];
        }
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    double ms_lsh = std::chrono::duration<double, std::milli>(t4 - t3).count();

    cout << "Point to examine: " << "(" ;
    for (double ax : point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    std::cout << "LSH (L=1) total: " << ms_lsh << endl;

    cout << "**********************************************************" << endl;

    return 0;

} 
// RESULTS:S
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