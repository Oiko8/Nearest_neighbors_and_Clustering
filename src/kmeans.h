#pragma once
#include "vectors.h"
#include <vector>
#include <random>

struct KMeans {
    // returns centroids as flat vector (k * d)
    static std::vector<float> train(const Dataset &ds, size_t k, unsigned seed=1, size_t max_iters=100, float tol=1e-4);
    static void assign_all(const Dataset &ds, const std::vector<float> &centroids, std::vector<uint32_t> &assignments);
};
