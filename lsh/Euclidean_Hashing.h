#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>

using namespace std;
typedef mt19937_64 random_generator;


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

class AmplifiedHash {
public:
    AmplifiedHash(int k, double w, int tableSize, random_generator& gen, int dim);
    int get_amplified_id(vector<double>& p) const;
    int get_point_id(vector<double>& p) const;

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



double euclidean_distance(const vector<double> &p1, const vector<double> &p2);