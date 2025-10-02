#include <unordered_map>

#include "Euclidean_Hashing.h"

using namespace std;
typedef mt19937_64 random_generator;


/* =============================== helper function to calculate euclidean distance ============================ */
double euclidean_distance(const vector<double> &p1, const vector<double> &p2){
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    return sqrt(dx*dx + dy*dy);
}


/* =============================================================================================== */
/* ========================================= Main ================================================ */
/* =============================================================================================== */

int main() {
    static random_generator generator(42);

    // Find the projection of a random point on the vector and hashing this point into bucket
    vector<double> p{5.3, 2.1};
    

    // window of each bucket
    int w = 2;

    /////////////////////////////////// small test ////////////////////////////////////////
    vector<vector<double>> pts = {
    // --- Cluster A: around (~0.4, 2.1) ---
    {0.30, 2.10},
    {0.35, 2.15},
    {0.40, 2.05},
    {0.90, 2.50},
    {0.45, 2.20},
    {0.32, 2.08},
    {0.37, 2.12},
    {0.41, 2.18},
    {0.28, 2.00},
    {0.52, 2.30},
    {0.48, 2.05},
    {0.60, 2.40},
    {0.33, 2.25},
    {0.55, 2.15},
    {0.42, 1.95},

    // --- Cluster D: around (~5.3, 2.1) ---
    {5.30, 2.10},
    {5.10, 2.00},
    {5.50, 2.20},
    {5.40, 1.90},
    {5.25, 2.30},
    {5.60, 2.05},
    {5.15, 2.25},
    {5.45, 2.15},

    // --- Cluster B: around (~10, -7) ---
    {10.00, -7.00},
    {9.50,  -6.80},
    {10.20, -7.10},
    {9.80,  -7.30},
    {10.40, -6.70},
    {9.60,  -6.50},
    {10.10, -6.90},
    {9.70,  -7.20},
    {10.50, -7.40},
    {9.90,  -6.60},
    {10.30, -6.95},
    {9.55,  -6.95},

    // --- Cluster C: around (~-3, 4) ---
    {-3.00, 4.00},
    {-2.80, 3.70},
    {-3.10, 4.20},
    {-2.90, 3.85},
    {-3.25, 3.95},
    {-2.70, 3.60},
    {-3.40, 4.10},
    {-3.05, 3.75},
    {-2.95, 4.25},
    {-3.20, 3.80},

    // --- Outliers / scattered ---
    {2.00, -1.00},
    {-1.50, -0.80},
    {7.80,  5.20},
    {-6.20, 1.40},
    {3.30, -4.50}
};


    w = 3;
    int k = 5;
    int tableSize = pts.size()/2;
    AmplifiedHash amplified_h(k, w, tableSize, generator);
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

    vector<double> point{5.4, 2.1};
    // Search for nearest neighor for a point
    
    cout << "Search for nearest neighor for a point" << endl;
    double min_dist = INFINITY; 
    double dist;
    vector<double> closest_point(2);
    for (vector<double> p : pts){
        dist = euclidean_distance(p, point);
        if (dist < min_dist){
            min_dist = dist;
            closest_point = p;
        }
    }  
    cout << "Point to examine: " << "(" << point[0] << "," << point[1] << ")" << endl;
    cout << "Closest point: " << "(" << closest_point[0] << "," << closest_point[1] << ")" << endl;
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
    cout << "Point to examine: " << "(" << point[0] << "," << point[1] << ")" << endl;
    cout << "Closest point: " << "(" << closest_point[0] << "," << closest_point[1] << ")" << endl;
    cout << "Distance: " << min_dist << endl;

    cout << "**********************************************************" << endl;

    return 0;

} 



