#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <queue>

/* ===================================== common definitions ====================================== */
using namespace std;
using random_generator = mt19937_64;

/* ======================================== hash class =========================================== */
class Hash {
    public:
    Hash(double w, int dim);
    int get_hash_id(vector<double>& p) const;
    
    private:
    vector<double> v_;
    double t_;
    double w_;
    int dim_;
    
    // helpers (declared here, defined in .cpp)
    vector<double> vec_d();
    static double normal_distribution_generator();
    static double generate_t(double w);
    static double dot(const vector<double>& v, const vector<double>& p);
};



/* ================================== Bit Mapper Class Implementation ============================ */
// functions f family to convert a hash function result to 0 or 1
// fi(hi) = {0,1}

class BitMapper {
public:
    // k = number of hypercube bits (same as your k'), seed for determinism
    BitMapper(int k, uint32_t seed = 1);

    // Map the j-th hash output h (an integer) to {0,1}
    // 0 <= j < k
    inline uint32_t bit_for(int j, int h) const {
        // Fast, stateless per-call mix using per-bit random A[j], B[j]
        uint32_t u = static_cast<uint32_t>(h);
        uint32_t x = A_[j] * u + B_[j];
        return mix32(x) & 1u; // 0 or 1
    }

    inline int bits() const { return k_; }

private:
    int k_;
    vector<uint32_t> A_, B_; // per-bit random coefficients

    static inline uint32_t mix32(uint32_t x){
        // Good 32-bit mixer (xorshift* / wyhash-inspired)
        x ^= x >> 16; 
        x *= 0x7feb352dU;
        x ^= x >> 15; 
        x *= 0x846ca68bU;
        x ^= x >> 16; 
        return x;
    }
};



#endif