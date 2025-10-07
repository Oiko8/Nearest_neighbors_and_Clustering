#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>
#include <unordered_map>
#include <chrono>

using namespace std;
typedef mt19937_64 random_generator;


/* =============================================================================================== */
/* =================================== Function Declaration ====================================== */
/* =============================================================================================== */

vector<double> vec_2d();
double normal_distribution_generator(void);
double dot(vector<double> v, vector<double> p);
double generate_t(int w);
int hash_function(vector<double> p, int w);


/* =============================================================================================== */
/* =================================== Functions Implementation ================================== */
/* =============================================================================================== */

/* ========= Helper function for the dot product of one a point and a vector ======== */
double dot(vector<double> v, vector<double> p){
    return v[0]*p[0] + v[1]*p[1];
}


/* ========= Creating a 2-vector with normal disrtibution ============== */
vector<double> vec_2d(){
    vector<double> v(2);
    v[0] = normal_distribution_generator();
    v[1] = normal_distribution_generator();
    return v;
}

/* ========= return a point with normal distribution =================== */
double normal_distribution_generator(void){
    static random_generator generator(42);
    normal_distribution<double> dist(0.0, 1.0);

    return dist(generator);
}


/* ========== generate a small disturbance t =========================== */
double generate_t(int w) {
    static random_generator generator(42);
    uniform_real_distribution<double> dist(0.0, w);
    return dist(generator);  
    
}


/* ===== Calculate the result of a random hash function h.i of H ======== */
int hash_function(vector<double> p, int w){
    // calculate the disturbance t accordind to the window w of each bucket
    double t = generate_t(w);
    // generate a random vector 2d with normal distribution
    vector<double> v;
    v = vec_2d();

    int h = floor((dot(v,p) + t)/w);
    return h;
}



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
/* ============================ Amplified Hash Class Implementation ============================== */
/* =============================================================================================== */

class AmplifiedHash {
public:
    AmplifiedHash(int k, double w, int tableSize, random_generator &gen){
        tableS_ = tableSize;
        // build k different hash funtions
        k_ = k;
        h.reserve(k_);
        r.reserve(k_);

        for (size_t i=0 ; i<k_ ; i++){
            // create new hash funtion
            Hash new_h(w, gen);
            h.push_back(new_h);
            // generate new r for the hash function
            r.push_back(generate_r(M_, gen)); 
        }

    }

    /* =================== calculate the id of the amplified has function ======================= */
    int get_amplified_id(vector<double> &p){
        unsigned long long hash_sum_ = 0;
        for (size_t i=0 ; i<k_ ; i++){
            hash_sum_ += r[i] * h[i].get_hash_id(p);
        }

        return (hash_sum_ % M_)% tableS_;
    }

private:
    int tableS_;
    int k_;
    vector<unsigned long long> r;       
    unsigned long long M_ = pow(2,32) - 5;;             

    vector<Hash> h;

    /* =============== generate r~[0, M) to multiply each hash result =========================== */
    static unsigned long long generate_r(unsigned long long M, random_generator &generator) {
        uniform_int_distribution<unsigned long long> dist(0.0, M);
        return dist(generator);  
    }

    

};




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

double euclidean_distance(const vector<double> &p1, const vector<double> &p2);

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
