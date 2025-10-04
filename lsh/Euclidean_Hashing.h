#ifndef EUCLIDEAN_HASHING_H
#define EUCLIDEAN_HASHING_H

#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>
#include <unordered_map>

/* ============================= common definitions ========================= */
using namespace std;
using random_generator = mt19937_64;
using Table = unordered_map<int, vector<int>>;


/* ============================== hash class ================================ */
class Hash {
    public:
    Hash(double w, random_generator& gen, int dim);
    int get_hash_id(vector<double>& p) const;
    
    private:
    vector<double> v_;
    double t_;
    double w_;
    int dim_;
    
    // helpers (declared here, defined in .cpp)
    vector<double> vec_d(random_generator& generator);
    static double normal_distribution_generator(random_generator& generator);
    static double generate_t(double w, random_generator& generator);
    static double dot(const vector<double>& v, const vector<double>& p);
};


/* ======================== amplified hash class ============================= */
class AmplifiedHash {
    public:
    AmplifiedHash(int k, double w, int tableSize, random_generator& gen, int dim);
    int get_amplified_id(vector<double>& p) const;
    int get_point_id(vector<double>& p) const;
    int getTableSize() const;
    
    private:
    int tableS_;
    int k_;
    int dim_;
    vector<unsigned long long> r_;
    unsigned long long M_ = 4294967291ULL; // exact (2^32 - 5)
    vector<Hash> h_;
    
    static unsigned long long generate_r(unsigned long long M, random_generator& generator);
    static unsigned long long pos_mod(long long x, unsigned long long M);
};



/* ========================= helper functions =============================== */
void build_tables(const vector<vector<double>> &pts);
vector<int> collect_possible_nn(const vector<double> &q);
int query_nn(const vector<vector<double>> &pts, const vector<double> &q);

double euclidean_distance(const vector<double> &p1, const vector<double> &p2);



/* ======================== global variables ================================ */
extern vector<AmplifiedHash> gHashes;
extern vector<Table> tables;
extern vector<vector<int>> point_ids;
extern vector<vector<int>> point_bucket_ids;


#endif