#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>

using namespace std;
typedef mt19937_64 random_generator;


/* =============================================================================================== */
/* ================================== Hash Class Implementation ================================== */
/* =============================================================================================== */

class Hash {
    public:
    Hash(double w, random_generator &gen) {
        w_ = w;
        t_ = generate_t(w_, gen);
        v_ = vec_2d(gen);
    }
    
    int get_hash_id(vector<double> &p) const {
        int h = static_cast<int>(floor((dot(v_,p) + t_)/w_));
        return h;
    }
    
    private:
    vector <double> v_;
    double t_;
    double w_;
    
    /* ======================= Helper functions ============================ */
    /* ========= Creating a 2-vector with normal disrtibution ============== */
    static vector<double> vec_2d(random_generator &generator){
        vector<double> v(2);
        v[0] = normal_distribution_generator(generator);
        v[1] = normal_distribution_generator(generator);
        return v;
    }
    
    /* ========= return a point with normal distribution =================== */
    static double normal_distribution_generator(random_generator &generator){
        
        normal_distribution<double> dist(0.0, 1.0);
        
        return dist(generator);
    }
    
    /* ========== generate a small disturbance t =========================== */
    static double generate_t(double w, random_generator &generator) {
        uniform_real_distribution<double> dist(0.0, w);
        return dist(generator);  
        
    }
    
    static double dot(const vector<double> &v, const vector<double> &p){
        return v[0]*p[0] + v[1]*p[1];
    }
    
};

/* =============================================================================================== */
/* ========================================= Main ================================================ */
/* =============================================================================================== */

int main() {
    static random_generator generator(42);

    // Find the projection of a random point on the vector and hashing this point into bucket
    vector<double> p{5.3, 2.1};
    

    // window of each bucket
    int w = 2;

    Hash h1(w, generator);
    int hash_id1 = h1.get_hash_id(p);
    cout << hash_id1 << endl;

    Hash h2(w, generator);
    int hash_id2 = h2.get_hash_id(p);
    cout << hash_id2 << endl;


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
    Hash h(w, generator);
    map <int, vector<int>> table;

    for (int i = 0; i < (int)pts.size(); ++i) {
        table[h.get_hash_id(pts[i])].push_back(i);
    }

    // using auto to let the compiler decides the type
    for (const auto& [bucket, ids] : table) {
        cout << "bucket " << bucket << " ->";
        for (int id : ids) {
            cout << " " << id;
        }
        cout << "\n";
    }


    return 0;
} 



