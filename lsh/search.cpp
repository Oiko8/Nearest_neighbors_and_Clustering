#include "Euclidean_Hashing.h"
#include <fstream>

vector<vector<double>> load_mnist_dataset(const string &filename) {
    ifstream f(filename, ios::binary);
    if (!f.is_open()) {
        cerr << "Error: cannot open " << filename << endl;
        exit(1);
    }

    uint32_t magic, num_images, rows, cols;
    f.read(reinterpret_cast<char*>(&magic), 4);
    f.read(reinterpret_cast<char*>(&num_images), 4);
    f.read(reinterpret_cast<char*>(&rows), 4);
    f.read(reinterpret_cast<char*>(&cols), 4);

    // convert from big endian to host
    auto swap32 = [](uint32_t x) {
        return ((x & 0xFF) << 24) |
               ((x & 0xFF00) << 8) |
               ((x & 0xFF0000) >> 8) |
               ((x >> 24) & 0xFF);
    };
    magic = swap32(magic);
    num_images = swap32(num_images);
    rows = swap32(rows);
    cols = swap32(cols);

    int dim = rows * cols;
    vector<vector<double>> data(num_images, vector<double>(dim));

    for (uint32_t i = 0; i < num_images; ++i) {
        for (int j = 0; j < dim; ++j) {
            unsigned char pixel;
            f.read(reinterpret_cast<char*>(&pixel), 1);
            data[i][j] = static_cast<double>(pixel);
        }
    }
    return data;
}


//$./search –d <input file> –q <query file> –k <int> -L <int> -w <double>
//          -ο <output file> -Ν <number of nearest> -R <radius> 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input.dat>" << endl;
        return 1;
    }
    string input_file = argv[1];

    cout << "Loading dataset: " << input_file << endl;
    vector<vector<double>> pts = load_mnist_dataset(input_file);
    cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;

    // pick the first image as a query
    vector<double> q = pts[0];
    int L = 4, khash = 3, k = 5;
    double w = 4.0;

    build_hash_tables(pts, L, khash, w);
    vector<int> nn_idx = query_knn(pts, q, k);

    cout << "*************************************************\n";
    cout << "Nearest neighbors for image 0:\n";
    for (int idx : nn_idx) cout << idx << " ";
    cout << endl;

    cout << "*************************************************\n";
    int R = 20000;
    vector<int> in_range_idx = range_search(pts, q, R);
    for (int idx : in_range_idx) cout << idx << " ";
    cout << endl;
}
