// Usage : ./search -d ../MNIST_data/input.dat -k 4 -w 4.0 -N 5 -R 2500

#include "Euclidean_Hashing.h"
#include <fstream>

struct Args {
    string data_path;         // -d
    string query_path;        // -q (optional)
    string type = "mnist";    // -type mnist|sift
    int k = 1;                // -N
    int L = 5;                // -L
    int khash = 4;            // -k
    double w = 4.0;           // -w
    double R = 2000.0;        // -R (MNIST default)
};

bool str2bool(const string& s){
    return (s=="1"||s=="true"||s=="True"||s=="TRUE");
}

Args parse_args(int argc, char** argv){
    Args a;
    for (int i=1; i<argc; ++i){
        string flag = argv[i];
        auto need = [&](int more){ if(i+more>=argc) { cerr<<"Missing value after "<<flag<<"\n"; exit(1);} };
        if (flag=="-d") { need(1); a.data_path=argv[++i]; }
        else if (flag=="-q") { need(1); a.query_path=argv[++i]; }
        else if (flag=="-type"){ need(1); a.type=argv[++i]; }
        else if (flag=="-N") { need(1); a.k=stoi(argv[++i]); }
        else if (flag=="-L") { need(1); a.L=stoi(argv[++i]); }
        else if (flag=="-k") { need(1); a.khash=stoi(argv[++i]); }
        else if (flag=="-w") { need(1); a.w=stod(argv[++i]); }
        else if (flag=="-R") { need(1); a.R=stod(argv[++i]); }
        
        else { cerr<<"Unknown flag: "<<flag<<"\n"; exit(1);}
    }
    if (a.data_path.empty()){
        cerr<<"Usage: "<<argv[0]<<" -d <input.dat> [-q <query.dat>] [-type mnist|sift] "
              "[-k <khash>] -L <L> -w <w> -N <N> -R <R> -range <true|false> -lsh\n";
        exit(1);
    }
    // Defaults per spec: MNIST R=2000, SIFT R=2
    if (a.type=="sift" && a.R==2000.0) a.R = 2.0;
    return a;
}

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


//$./search –d <input file> –q <query file> –k <int> -L <int> -w <double> -ο <output file> -Ν <number of nearest> -R <radius> 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " -d <input file> -q <query file> -k <int> -L <int> -w <double>" 
                                     << " -o <output file> -N <number of nearest> -R <radius> " << endl;
        return 1;
    }
    Args args = parse_args(argc, argv);

    string input_file = args.data_path;

    cout << "Loading dataset: " << input_file << endl;
    vector<vector<double>> pts = load_mnist_dataset(input_file);
    cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;

    // pick the first image as a query
    vector<double> q = pts[0];
    int L = args.L, khash = args.khash, k = args.k;
    double w = args.w;

    build_hash_tables(pts, L, khash, w);
    vector<int> nn_idx = query_knn(pts, q, k);

    cout << "*************************************************\n";
    cout << "Nearest neighbors for image 0:\n";
    for (int idx : nn_idx) cout << idx << " ";
    cout << endl;

    cout << "*************************************************\n";
    int R = args.R;
    vector<int> in_range_idx = range_search(pts, q, R);
    for (int idx : in_range_idx) cout << idx << " ";
    cout << endl;
}
