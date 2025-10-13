#include <fstream>
#include <iostream>
#include <cstdlib>    
#include <random>      
#include <vector>
using namespace std;

// load the mnist dataset
vector<vector<double>> load_mnist_dataset(const string &filename);
// load the sift dataset
vector<vector<double>> load_sift_dataset(const string& filename);