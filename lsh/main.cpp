#include <unordered_map>

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


/* =============================================================================================== */
/* ========================================= Main ================================================ */
/* =============================================================================================== */

int main() {
    static random_generator generator(42);

   
    /////////////////////////////////// small test ////////////////////////////////////////
    // vector<vector<double>> pts = {
    //     // --- Cluster A: around (~0.4, 2.1) ---
    //     {0.30, 2.10},
    //     {0.35, 2.15},
    //     {0.40, 2.05},
    //     {0.90, 2.50},
    //     {0.45, 2.20},
    //     {0.32, 2.08},
    //     {0.37, 2.12},
    //     {0.41, 2.18},
    //     {0.28, 2.00},
    //     {0.52, 2.30},
    //     {0.48, 2.05},
    //     {0.60, 2.40},
    //     {0.33, 2.25},
    //     {0.55, 2.15},
    //     {0.42, 1.95},

    //     // --- Cluster D: around (~5.3, 2.1) ---
    //     {5.30, 2.10},
    //     {5.10, 2.00},
    //     {5.50, 2.20},
    //     {5.40, 1.90},
    //     {5.25, 2.30},
    //     {5.60, 2.05},
    //     {5.15, 2.25},
    //     {5.45, 2.15},

    //     // --- Cluster B: around (~10, -7) ---
    //     {10.00, -7.00},
    //     {9.50,  -6.80},
    //     {10.20, -7.10},
    //     {9.80,  -7.30},
    //     {10.40, -6.70},
    //     {9.60,  -6.50},
    //     {10.10, -6.90},
    //     {9.70,  -7.20},
    //     {10.50, -7.40},
    //     {9.90,  -6.60},
    //     {10.30, -6.95},
    //     {9.55,  -6.95},

    //     // --- Cluster C: around (~-3, 4) ---
    //     {-3.00, 4.00},
    //     {-2.80, 3.70},
    //     {-3.10, 4.20},
    //     {-2.90, 3.85},
    //     {-3.25, 3.95},
    //     {-2.70, 3.60},
    //     {-3.40, 4.10},
    //     {-3.05, 3.75},
    //     {-2.95, 4.25},
    //     {-3.20, 3.80},

    //     // --- Outliers / scattered ---
    //     {2.00, -1.00},
    //     {-1.50, -0.80},
    //     {7.80,  5.20},
    //     {-6.20, 1.40},
    //     {3.30, -4.50}
    // };

    vector<std::vector<double>> pts = {
        // --- Cluster A: around (~0.4, 2.1, 0.5) ---
        {0.30, 2.10, 0.40},
        {0.35, 2.15, 0.55},
        {0.40, 2.05, 0.60},
        {0.90, 2.50, 0.30},
        {0.45, 2.20, 0.45},
        {0.32, 2.08, 0.35},
        {0.37, 2.12, 0.50},
        {0.41, 2.18, 0.65},
        {0.28, 2.00, 0.25},
        {0.52, 2.30, 0.55},
        {0.48, 2.05, 0.60},
        {0.60, 2.40, 0.20},
        {0.33, 2.25, 0.70},
        {0.55, 2.15, 0.40},
        {0.42, 1.95, 0.35},

        // --- Cluster D: around (~5.3, 2.1, 1.0) ---
        {5.30, 2.10, 1.10},
        {5.10, 2.00, 0.90},
        {5.50, 2.20, 1.05},
        {5.40, 1.90, 0.80},
        {5.25, 2.30, 1.20},
        {5.60, 2.05, 0.95},
        {5.15, 2.25, 1.30},
        {5.45, 2.15, 1.00},

        // --- Cluster B: around (~10, -7, -2.0) ---
        {10.00, -7.00, -2.10},
        {9.50,  -6.80, -2.20},
        {10.20, -7.10, -1.80},
        {9.80,  -7.30, -2.40},
        {10.40, -6.70, -2.25},
        {9.60,  -6.50, -1.90},
        {10.10, -6.90, -2.30},
        {9.70,  -7.20, -1.95},
        {10.50, -7.40, -2.50},
        {9.90,  -6.60, -1.85},
        {10.30, -6.95, -2.20},
        {9.55,  -6.95, -2.10},

        // --- Cluster C: around (~-3, 4, 2.0) ---
        {-3.00, 4.00, 2.00},
        {-2.80, 3.70, 2.10},
        {-3.10, 4.20, 1.90},
        {-2.90, 3.85, 2.25},
        {-3.25, 3.95, 2.05},
        {-2.70, 3.60, 1.95},
        {-3.40, 4.10, 2.20},
        {-3.05, 3.75, 2.30},
        {-2.95, 4.25, 1.80},
        {-3.20, 3.80, 2.15},

        // --- Outliers / scattered ---
        {2.00, -1.00, 3.00},
        {-1.50, -0.80, -2.50},
        {7.80,  5.20, 0.50},
        {-6.20, 1.40, 1.80},
        {3.30, -4.50, -0.75}
    };



    int dim = pts[0].size();
    double w = 3;
    int k = 5;
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

    vector<double> point{5.4, 2.1, 1.15};
    // Search for nearest neighor for a point
    
    cout << "Search for nearest neighor for a point" << endl;
    double min_dist = INFINITY; 
    double dist;
    vector<double> closest_point(dim);
    for (vector<double> p : pts){
        dist = euclidean_distance(p, point);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = p;
        }
    }  
    cout << "Point to examine: " << "(" ;
    for (double ax : point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;


    cout << "**********************************************************" << endl;
    // Search for nearest neighor for a point USING THE HASH TABLE
    cout << "Search for nearest neighor for a point through the hash table" << endl;
    int bucket_of_point = amplified_h.get_amplified_id(point);
    // cout << bucket_of_point << endl;

    min_dist = INFINITY;
    for (int id : table[bucket_of_point]) {
        dist = euclidean_distance(point, pts[id]);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = pts[id];
        }
    }
    cout << "Point to examine: " << "(" ;
    for (double ax : point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Closest point: " << "(" ;
    for (double ax : closest_point) {cout << ax << ",";}
    cout << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    cout << "**********************************************************" << endl;

    return 0;

} 



