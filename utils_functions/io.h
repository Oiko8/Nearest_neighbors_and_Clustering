#pragma once    // apofeugoume pollapli eisagwgh tou arxeiou (header guard)

#include "../src/ivf/vectors.h"   // perilamvanei ton orismo tou struct Dataset
#include <string>   // xreiazetai gia xrisi typou std::string

// Readers gia MNIST (images as 28x28 unsigned bytes, big-endian header)
// kai gia fvecs (SIFT) - .fvecs format: int dim (little-endian), then floats

bool read_mnist_images(const std::string &path, Dataset &out);      // synarthsh pou diavazei arxeio MNIST kai gemizei to Dataset me eikones
bool read_fvecs(const std::string &path, Dataset &out);     // synarthsh pou diavazei arxeio SIFT .fvecs kai gemizei to Dataset me dianysmata float
