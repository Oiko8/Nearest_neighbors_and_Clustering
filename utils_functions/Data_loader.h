#pragma once    // apofeugoume pollapli eisagwgh tou arxeiou (header guard)

#include "../src/ivf/vectors.h"   // perilamvanei ton orismo tou struct Dataset
#include <string>   // xreiazetai gia xrisi typou std::string

// Readers gia MNIST (images as 28x28 unsigned bytes, big-endian header)
// kai gia fvecs (SIFT) - .fvecs format: int dim (little-endian), then floats


#include <fstream>
#include <iostream>
#include <cstdlib>    
#include <random>      
#include <vector>
using namespace std;


// load the mnist dataset
vector<vector<float>> load_mnist_dataset(const string &filename);
// load the sift dataset
vector<vector<float>> load_sift_dataset(const string& filename);

// reading the files for ivflat. Another implementation for better data management
bool read_mnist_images(const std::string &path, Dataset &out);      
bool read_fvecs(const std::string &path, Dataset &out);     