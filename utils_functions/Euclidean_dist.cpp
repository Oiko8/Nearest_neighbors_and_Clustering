#include "Euclidean_dist.h"

/* =============================== helper function to calculate euclidean distance ============================ */
double euclidean_distance(const vector<double> &p1, const vector<double> &p2){
    double sum = 0.0;
    int size = p1.size();
    for (int i = 0 ; i < size ; i++){
        sum += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return sqrt(sum);
}