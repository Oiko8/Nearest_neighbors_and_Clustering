#include "Euclidean_Hashing.h"

/* =============================================================================================== */
/* ================================== Hash Class Implementation ================================== */
/* =============================================================================================== */

Hash::Hash(double w, random_generator &gen) {
    w_ = w;
    t_ = generate_t(w_, gen);
    v_ = vec_2d(gen);
} 
    
int Hash::get_hash_id(vector<double>& p) const {
    int h = static_cast<int>(floor((dot(v_, p) + t_) / w_));
    return h;
}

/* ======================= Helper functions ============================ */
/* ========= Creating a 2-vector with normal disrtibution ============== */

vector<double> Hash::vec_2d(random_generator& generator){
    vector<double> v(2);
    v[0] = normal_distribution_generator(generator);
    v[1] = normal_distribution_generator(generator);
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
    return v[0]*p[0] + v[1]*p[1];
}

  
/* =============================================================================================== */
/* ============================ Amplified Hash Class Implementation ============================== */
/* =============================================================================================== */

AmplifiedHash::AmplifiedHash(int k, double w, int tableSize, random_generator& gen) {
    tableS_ = tableSize;
    k_ = k;
    h_.reserve(k_);
    r_.reserve(k_);

    for (int i = 0; i < k_; i++) {
        // create new hash function
        Hash new_h(w, gen);
        h_.push_back(new_h);
        // generate new r for the hash function
        r_.push_back(generate_r(M_, gen));   // r_i ~ [0, M-1]
    }
}

int AmplifiedHash::get_amplified_id(vector<double> &p) const{
    unsigned long long hash_sum_ = 0;
    for (int i=0 ; i<k_ ; i++){
        hash_sum_ += r_[i] * h_[i].get_hash_id(p);
    }

    return (hash_sum_ % M_)% tableS_;
}

/* =============== generate r~[0, M-1) to multiply each hash result =========================== */
unsigned long long AmplifiedHash::generate_r(unsigned long long M, random_generator& generator) {
    // integer coefficients r_i ∈ {0, …, M-1}
    uniform_int_distribution<unsigned long long> dist(0ULL, M - 1ULL);
    return dist(generator);
}




