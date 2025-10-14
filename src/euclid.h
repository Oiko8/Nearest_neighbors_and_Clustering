#pragma once    // apofevgoume na perilifthei to arxeio pollaples fores (header guard)

#include <cstddef>  // periexei orismous typwn opws to size_t
#include <cmath>    // periexei mathimatikes synarthseis (px pow, sqrt)

// dilwsi inline synarthshs gia ypologismo tetragwnikhs apostashs L2 (euclidean distance)
inline float eucliddistance(const float* a, const float* b, size_t d) {

    float s = 0.0f; // arxikopoihsh synolou (sum) me 0.0 float

    for(size_t i=0;i<d;i++){    // gia kathe diastash apo 0 mexri d-1
        float diff = a[i]-b[i]; // upologizoume th diafora metaksy tou stoixeiou a kai b
        s += diff*diff;         // prosthetoume sto sum to tetragwno ths diaforas
    }

    return s;                   // epistrefoume to athroisma twn tetragwnwn (oxi sqrt, ara squared L2)
}
