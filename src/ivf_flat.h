#pragma once
#include "vectors.h"
#include <vector>

struct IVFFlat {
    size_t kclusters = 0;
    size_t d = 0;
    std::vector<float> centroids; // kclusters * d
    std::vector<std::vector<uint32_t>> lists; // per centroid: ids

    void train_and_index(const Dataset &ds, size_t kclusters, unsigned seed=1);
    std::vector<std::pair<uint32_t, float>> search(const float* q, const Dataset &ds, size_t N, size_t nprobe, float R = -1.0f) const;
};
