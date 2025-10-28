// testing the parameters

#include "Euclidean_Hashing.h"
#include "../../utils_functions/Data_loader.h"
#include "../../utils_functions/euclid.h"
#include <fstream>

#define PATH "../SIFT_data/input.dat"

int main() {
    vector<vector<float>> pts;
    vector<vector<float>> queries;

    // From the input file extract the data and transform the correct way
    string input_file = PATH;
    cout << "Loading dataset: " << input_file << endl;
    pts = load_sift_dataset(input_file);
    cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;

    cout << "*************************************************\n";

    for (auto v : pts[0]) cout << v << "\n";
    cout << endl;
    
    return 0;
}