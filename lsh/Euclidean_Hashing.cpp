#include "Euclidean_Hashing.h"

/*=================================== Global definitions =========================================*/
vector<AmplifiedHash> amplified_functions;
vector<Hash> hash_functions;
vector<Table> tables;
vector<vector<int>> point_ids;
vector<vector<int>> point_bucket_ids;
random_generator gen(42);


/* =============================================================================================== */
/* ======================================== The Query ============================================ */
/* =============================================================================================== */
vector<double> query_knn(const vector<vector<double>> &pts, vector<double> &q, int k){
    int dim = q.size();
    int L = tables.size();
    // vector<vector<double>> closest_points(L, vector<double>(dim));
    vector <double> closest_point(dim);

    double min_dist = INFINITY;
    double dist;

    for (int i = 0 ; i < L ; i++) {
        int bucket_of_query = amplified_functions[i].get_amplified_id(q); 
        min_dist = INFINITY;
        for (int id : tables[i][bucket_of_query]) {
            dist = euclidean_distance(q, pts[id]);
            if (dist < min_dist){
                min_dist = dist;
                closest_point = pts[id];
            }
        }
        cout << "**********************************************************" << endl;
        cout << "Point to examine: " << "(" ;
        for (double ax : q) {cout << ax << ",";}
        cout << ")" << endl;
        cout << "Closest point: " << "(" ;
        for (double ax : closest_point) {cout << ax << ",";}
        cout << ")" << endl;
        cout << "Distance: " << min_dist << endl;
        cout << "**********************************************************" << endl;
    }
    

    return closest_point;
}




/* =============================================================================================== */
/* ===================================== Creating the Tables ===================================== */
/* =============================================================================================== */
void build_hash_tables(vector<vector<double>> &pts, int L, int khash, double w){
    int tableSize = pts.size()/2;   // no of buckets in each table
    int dim = pts[0].size();

    tables.clear();
    tables.resize(L);

    amplified_functions.clear();
    amplified_functions.resize(L);

    for (int i = 0 ; i < L ; i ++) {
        amplified_functions[i] = AmplifiedHash(khash, w, tableSize, dim);
    }
    
    for (int l = 0; l < L ; l++) {
        for (int i = 0; i < (int)pts.size(); ++i) {
            tables[l][amplified_functions[l].get_amplified_id(pts[i])].push_back(i);
        }
    } 
}



/* =============================================================================================== */
/* ================================== Hash Class Implementation ================================== */
/* =============================================================================================== */

Hash::Hash(double w, int dim) {
    w_ = w;
    dim_ = dim;
    t_ = generate_t(w_);
    v_ = vec_d();
} 
    
int Hash::get_hash_id(vector<double>& p) const {
    int h = static_cast<int>(floor((dot(v_, p) + t_) / w_));
    return h;
}

/* ======================= Helper functions ============================ */
/* ========= Creating a 2-vector with normal disrtibution ============== */

vector<double> Hash::vec_d(){
    vector<double> v(dim_);
    for (int i=0 ; i < dim_ ; i++){
        v[i] = normal_distribution_generator();
    }
    return v;
}

/* ========= return a point with normal distribution =================== */
double Hash::normal_distribution_generator(){
    normal_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

/* ========== generate a small disturbance t =========================== */
double Hash::generate_t(double w) {
    // keep your original definition (Uniform[0, w])
    uniform_real_distribution<double> dist(0.0, w);
    return dist(gen);
}

double Hash::dot(const vector<double>& v, const vector<double>& p){
    double dot_product = 0.0;
    int size = v.size();
    for (int i=0 ; i < size ; i++){
        dot_product += v[i]*p[i];
    } 
    return dot_product;
}

  
/* =============================================================================================== */
/* ============================ Amplified Hash Class Implementation ============================== */
/* =============================================================================================== */

AmplifiedHash::AmplifiedHash(int k, double w, int tableSize, int dim) {
    tableS_ = tableSize;
    k_ = k;
    h_.reserve(k_);
    r_.reserve(k_);
    dim_ = dim;

    for (int i = 0; i < k_; i++) {
        // create new hash function
        Hash new_h(w, dim_);
        h_.push_back(new_h);
        // generate new r for the hash function
        r_.push_back(generate_r(M_));   // r_i ~ [0, M-1]
    }
}

int AmplifiedHash::getTableSize() const { return tableS_; }


int AmplifiedHash::get_point_id(vector<double> &p) const {
    __int128 acc = 0;                           
    for (int i = 0; i < k_; ++i) {
        int hi = h_[i].get_hash_id(p);          // may be negative
        long long hnorm = hi % M_;
        if (hnorm < 0) hnorm += M_;             // now hnorm in [0, M-1]
        acc += static_cast<__int128>(r_[i]) * static_cast<__int128>(hnorm);
    }
    int id = static_cast<int>(static_cast<long long>(acc % M_));
    return id;
}

int AmplifiedHash::get_amplified_id(vector<double> &p) const{
    int id_ = get_point_id(p);

    return id_% tableS_;
}

/* =============== generate r~[0, M-1) to multiply each hash result =========================== */
unsigned long long AmplifiedHash::generate_r(unsigned long long M) {
    // integer coefficients r_i ∈ {0, …, M-1}
    uniform_int_distribution<unsigned long long> dist(0ULL, M - 1ULL);
    return dist(gen);
}



/* =============================================================================================== */
/* ====================================== Helper Functions ======================================= */
/* =============================================================================================== */

/* =============================== helper function to calculate euclidean distance ============================ */
double euclidean_distance(const vector<double> &p1, const vector<double> &p2){
    double sum = 0.0;
    int size = p1.size();
    for (int i = 0 ; i < size ; i++){
        sum += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return sqrt(sum);
}




