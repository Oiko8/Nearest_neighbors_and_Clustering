#include "Hypercube.h"

/*=================================== Global definitions =========================================*/
random_generator gen(42);
vector<Hash> hash_functions;
BitMapper bit_function(0);
Table Hypercube_table;


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
/* ================================== Bit Mapper Class Implementation ============================ */
/* =============================================================================================== */

BitMapper::BitMapper(int k, uint32_t seed)
: k_(k), A_(k), B_(k)
{
    mt19937 rng(seed);
    uniform_int_distribution<uint32_t> u32;
    for (int j = 0; j < k_; ++j) {
        A_[j] = (u32(rng) | 1u); // odd -> better spreading
        B_[j] = (u32(rng) | 1u);
    }
}



/* ================================= Build the hypercude ========================================= */
void build_hypercube(vector<vector<double>> &pts, int k, double w){
    int dim = static_cast<int>(pts[0].size());

    for (int i = 0 ; i < k ; i++) {
        Hash new_h(w, dim);
        hash_functions.push_back(new_h);

    }

    bit_function = BitMapper(k);
    for (int idx = 0 ; idx < static_cast<int>(pts.size()) ; idx++) {
        string vertex = "";
        for (int i = 0 ; i < k ; i++) {
            int h_id = hash_functions[i].get_hash_id(pts[idx]);
            auto bit = bit_function.bit_for(i, h_id);
            vertex += (bit ? "1" : "0"); 
        }

        Hypercube_table[vertex].push_back(idx);
    }
}