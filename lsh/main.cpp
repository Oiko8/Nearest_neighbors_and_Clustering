#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>
#include "Euclidean_Hashing.h"

using namespace std;
typedef mt19937_64 random_generator;





/* =============================================================================================== */
/* ========================================= Main ================================================ */
/* =============================================================================================== */

int main() {
    static random_generator generator(42);

    // Find the projection of a random point on the vector and hashing this point into bucket
    vector<double> p{5.3, 2.1};
    

    // window of each bucket
    int w = 2;

    // Hash h1(w, generator);
    // int hash_id1 = h1.get_hash_id(p);
    // cout << hash_id1 << endl;

    // Hash h2(w, generator);
    // int hash_id2 = h2.get_hash_id(p);
    // cout << hash_id2 << endl;


    /////////////////////////////////// small test ////////////////////////////////////////
    vector<vector<double>> pts = {
        {0.30, 2.10},
        {0.35, 2.15},
        {0.40, 2.05},
        {0.90, 2.50},
        {0.45, 2.20},  // cluster near (~0.4, 2.1)

        {5.30, 2.10},  // your earlier example

        {10.00, -7.00},
        {9.50,  -6.80}, // far cluster

        {-3.00, 4.00},
        {-2.80, 3.70}   // another region
    };

    w = 3;
    int k = 5;
    int tableSize = pts.size()/2;
    AmplifiedHash amplified_h(k, w, tableSize, generator);
    map <int, vector<int>> table;

    for (int i = 0; i < (int)pts.size(); ++i) {
        table[amplified_h.get_amplified_id(pts[i])].push_back(i);
    }

    // using auto to let the compiler decides the type
    for (const auto& [bucket, ids] : table) {
        cout << "bucket " << bucket << " ->";
        for (int id : ids) {
            cout << "  " ;
            cout << "(" << pts[id][0] << "," << pts[id][1] << ")";
        }
        cout << "\n";
    }


    return 0;
} 



