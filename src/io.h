#pragma once
#include "vectors.h"
#include <string>

// Readers for MNIST (images as 28x28 unsigned bytes, big-endian header)
// and for fvecs (SIFT) - .fvecs format: int dim (little-endian), then floats
bool read_mnist_images(const std::string &path, Dataset &out);
bool read_fvecs(const std::string &path, Dataset &out);
