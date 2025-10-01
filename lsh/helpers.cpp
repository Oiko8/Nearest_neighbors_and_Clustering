#include <iostream>
#include <cstdlib>    
#include <random>      
#include <ctime>
#include <vector>
#include <map>
#include <cmath>

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