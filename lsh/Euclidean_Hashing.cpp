#include "Euclidean_Hashing.h"

/* =============================================================================================== */
/* ================================== Hash Class Implementation ================================== */
/* =============================================================================================== */

Hash::Hash(double w, random_generator &gen, int dim) {
    w_ = w;
    dim_ = dim;
    t_ = generate_t(w_, gen);
    v_ = vec_d(gen);
} 
    
int Hash::get_hash_id(vector<double>& p) const {
    int h = static_cast<int>(floor((dot(v_, p) + t_) / w_));
    return h;
}

/* ======================= Helper functions ============================ */
/* ========= Creating a 2-vector with normal disrtibution ============== */

vector<double> Hash::vec_d(random_generator& generator){
    vector<double> v(dim_);
    for (int i=0 ; i < dim_ ; i++){
        v[i] = normal_distribution_generator(generator);
    }
    return v;
}

/* ========= return a point with normal distribution =================== */
double Hash::normal_distribution_generator(random_generator& generator){
    normal_distribution<double> dist(0.0, 1.0);
    return dist(generator);
}

/* ========== generate a small disturbance t =========================== */
double Hash::generate_t(double w, random_generator& generator) {
    // keep your original definition (Uniform[0, w])
    uniform_real_distribution<double> dist(0.0, w);
    return dist(generator);
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

AmplifiedHash::AmplifiedHash(int k, double w, int tableSize, random_generator& gen, int dim) {
    tableS_ = tableSize;
    k_ = k;
    h_.reserve(k_);
    r_.reserve(k_);
    dim_ = dim;

    for (int i = 0; i < k_; i++) {
        // create new hash function
        Hash new_h(w, gen, dim_);
        h_.push_back(new_h);
        // generate new r for the hash function
        r_.push_back(generate_r(M_, gen));   // r_i ~ [0, M-1]
    }
}


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
unsigned long long AmplifiedHash::generate_r(unsigned long long M, random_generator& generator) {
    // integer coefficients r_i ∈ {0, …, M-1}
    uniform_int_distribution<unsigned long long> dist(0ULL, M - 1ULL);
    return dist(generator);
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


