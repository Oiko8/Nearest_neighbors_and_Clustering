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
    
int Hash::get_hash_id(const vector<double>& p) const {
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


/* =============================================================================================== */
/* ===================================== Build the hypercude ===================================== */
/* =============================================================================================== */

void build_hypercube(const vector<vector<double>> &pts, int k, double w, uint32_t seed){
    int dim = static_cast<int>(pts[0].size());

    // (re)seed global RNG and reset state for rebuilds
    gen.seed(seed);
    hash_functions.clear();
    Hypercube_table.clear();

    for (int i = 0 ; i < k ; i++) {
        Hash new_h(w, dim);
        hash_functions.push_back(new_h);
    }

    bit_function = BitMapper(k, seed);

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


/* =============================================================================================== */
/* ===================================== NN and Range Search ===================================== */
/* =============================================================================================== */


/* ============================== Helper: compute a vertex for a query =====================================  */

string vertex_for_point(const vector<double> &p){
    int k = bit_function.bits();
    string v;
    v.reserve(k);
    for (int i = 0; i < k; ++i){
        int h_id = hash_functions[i].get_hash_id(p);
        uint32_t b = bit_function.bit_for(i, h_id);
        v += (b ? '1' : '0');
    }
    return v;
}

/* =================================== Helper: Hamming-order neighbor masks ================================ */
static inline int popcount32(uint32_t x){
    return __builtin_popcount(x);
}

vector<uint32_t> neighbor_masks_in_hamming_order(int k, int probes){
    // generate all masks up to 'probes' items, sorted by Hamming weight then by value
    // (k <= 32 assumed; if you go larger, switch to 64-bit)
    const uint32_t limit = (k >= 32) ? 0xFFFFFFFFu : ((1u << k) - 1u);

    vector<uint32_t> masks;
    masks.reserve(probes);
    // we’ll sweep weights 0..k and take until 'probes' is reached
    for (int w = 0; w <= k && static_cast<int>(masks.size()) < probes; ++w){
        for (uint32_t m = 0; m <= limit && static_cast<int>(masks.size()) < probes; ++m){
            if (popcount32(m) == w){
                masks.push_back(m);
            }
        }
    }
    return masks; // includes 0 mask first
}

static inline void flip_in_place(string &v, uint32_t mask){
    int k = static_cast<int>(v.size());
    for (int j = 0; j < k; ++j){
        if (mask & (1u << j)){
            v[j] = (v[j] == '0') ? '1' : '0';
        }
    }
}

static inline string xor_vertex_with_mask(const string &v, uint32_t mask){
    string out = v;
    flip_in_place(out, mask);
    return out;
}


/* =============================== Candidate gathering ============================================ */
static vector<int> gather_candidates(const vector<double> &q,
                                     int M, int probes)
{
    vector<int> cand;
    cand.reserve(M);
    unordered_set<int> seen;

    const string base = vertex_for_point(q);
    const int k = bit_function.bits();

    vector<uint32_t> masks = neighbor_masks_in_hamming_order(k, probes);

    for (uint32_t m : masks){
        if ((int)cand.size() >= M) break;
        string vtx = xor_vertex_with_mask(base, m);
        auto it = Hypercube_table.find(vtx);
        if (it == Hypercube_table.end()) continue;
        for (int id : it->second){
            if ((int)cand.size() >= M) break;
            if (seen.insert(id).second){
                cand.push_back(id);
            }
        }
    }
    return cand;
}

/* ========================================= KNN query ============================================ */
vector<int> cube_query_knn(const vector<vector<double>> &pts,
                           const vector<double> &q,
                           int N, int M, int probes)
{
    vector<int> cand = gather_candidates(q, M, probes);
    if (cand.empty()) return {};

    // max-heap of (dist, id); keep only best N
    priority_queue<pair<double,int>> heap;

    for (int id : cand){
        double d = euclidean_distance(q, pts[id]);
        if ((int)heap.size() < N){
            heap.emplace(d, id);
        } else if (d < heap.top().first){
            heap.pop();
            heap.emplace(d, id);
        }
    }

    // extract in ascending distance
    vector<pair<double,int>> tmp;
    tmp.reserve(heap.size());
    while (!heap.empty()){
        tmp.push_back(heap.top());
        heap.pop();
    }
    sort(tmp.begin(), tmp.end(), [](const auto &a, const auto &b){
        return a.first < b.first;
    });

    vector<int> nn;
    nn.reserve(tmp.size());
    for (auto &p : tmp) nn.push_back(p.second);
    return nn;
}

/* ======================================== Range query =========================================== */
vector<int> cube_query_range(const vector<vector<double>> &pts,
                             const vector<double> &q,
                             double R, int M, int probes)
{
    vector<int> cand = gather_candidates(q, M, probes);
    vector<int> inside;
    inside.reserve(cand.size());
    for (int id : cand){
        if (euclidean_distance(q, pts[id]) <= R){
            inside.push_back(id);
        }
    }
    return inside;
}


