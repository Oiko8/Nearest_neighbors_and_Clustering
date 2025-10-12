#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

using Idx = uint32_t;
using Vec = std::vector<float>;

struct Dataset {
    size_t n = 0;
    size_t d = 0;
    std::vector<float> data; // row-major: data[i*d + j]
    void reserve(size_t N, size_t D){ n=N; d=D; data.resize(N*D); }
    const float* row(size_t i) const { return &data[i * d]; }
    float* row_mut(size_t i) { return &data[i * d]; }
};
