#ifndef EUCLIDEAN_HASHING_H
#define EUCLIDEAN_HASHING_H

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

/* ============================= common definitions ========================= */
using namespace std;
using random_generator = mt19937_64;
using Table = unordered_map<int, vector<int>>;

// extern random_generator gen(42);


/* ============================== hash class ================================ */
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


/* ======================== amplified hash class ============================= */
class AmplifiedHash {
    public:
    AmplifiedHash() = default;
    AmplifiedHash(int k, double w, int tableSize, int dim);
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
    
    static unsigned long long generate_r(unsigned long long M);
    static unsigned long long pos_mod(long long x, unsigned long long M);
};



/* ========================= helper functions =============================== */
void build_hash_tables(vector<vector<double>> &pts, int L, int khash, double w);
vector<int> collect_possible_nn(const vector<double> &q);
vector<double> query_knn(const vector<vector<double>> &pts, vector<double> &q, int k);
vector<int> range_search(const vector<vector<double>> &pts, vector<double> &q, double R);

double euclidean_distance(const vector<double> &p1, const vector<double> &p2);



/* ======================== global variables ================================ */
extern vector<AmplifiedHash> amplified_functions;
extern vector<Hash> hash_functions;
extern vector<Table> tables;
extern vector<vector<int>> point_ids;
extern vector<vector<int>> point_bucket_ids;


#endif