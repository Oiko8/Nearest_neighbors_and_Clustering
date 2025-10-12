#pragma once
#include <cstddef>
#include <cmath>

inline float l2_sq(const float* a, const float* b, size_t d) {
    float s = 0.0f;
    for(size_t i=0;i<d;i++){
        float diff = a[i]-b[i];
        s += diff*diff;
    }
    return s;
}
