#include "ivf_pq.h"
#include "../utils_functions/BruteForceImplementation.h"
#include "../utils_functions/Rangesearch.h"
#include "io.h"
#include "vectors.h"
#include "../utils_functions/euclid.h"
#include "kmeans.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

using clock_type = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

struct Args {
    std::string method = "ivfpq";
    std::string data_path;
    std::string query_path;
    std::string type = "mnist";
    int N = 5;
    int kclusters = 50;
    int nprobe = 5;
    int M = 16;
    int nbits = 8;
    double R = -1.0;
    unsigned seed = 1;
};

Args parse_args(int argc, char** argv) { // function pou diabazei command line arguments
    Args a; // dimiourgia struct me default times
    for (int i = 1; i < argc; ++i) { // loop gia ola ta args
        std::string f = argv[i]; // pairnoume to trexon arg
        auto need = [&](int more){ if (i + more >= argc) { std::cerr << "Missing value after " << f << "\n"; exit(1); } }; // elegxos oti yparxei timi meta to flag
        if (f == "-method") { need(1); a.method = argv[++i]; }       // diavase -method
        else if (f == "-d") { need(1); a.data_path = argv[++i]; }    // diavase -d dataset
        else if (f == "-q") { need(1); a.query_path = argv[++i]; }   // diavase -q queries
        else if (f == "-type") { need(1); a.type = argv[++i]; }      // diavase -type
        else if (f == "-N") { need(1); a.N = std::stoi(argv[++i]); } // diavase -N number of neighbors
        else if (f == "-kclusters") { need(1); a.kclusters = std::stoi(argv[++i]); } // -kclusters
        else if (f == "-nprobe") { need(1); a.nprobe = std::stoi(argv[++i]); } // -nprobe
        else if (f == "-R") { need(1); a.R = std::stod(argv[++i]); }  // -R radius
        else if (f == "-seed") { need(1); a.seed = static_cast<unsigned>(std::stoul(argv[++i])); } // -seed
        else if (f=="-M"){ need(1); a.M = std::stoi(argv[++i]); }
        else if (f=="-nbits"){ need(1); a.nbits = std::stoi(argv[++i]); }
        else { std::cerr << "Unknown flag: " << f << "\n"; exit(1); } // an flag den uparxei terminate
    }
    if (a.method != "ivfflat" && a.method != "ivfpq") { // elegxos tou method
        std::cerr<<"Error: only 'ivfflat' and 'ivfpq' supported currently\n";
        exit(1);
    }
    if (a.data_path.empty() || a.query_path.empty()) { // elegxos twn paths
        std::cerr << "Usage: ... \n"; exit(1);
    }
    return a; // epistrefei struct me args
}
int ivfpq_main(int argc, char** argv) {
    Args args = parse_args(argc, argv);

    Dataset base, queries;
    bool ok = (args.type=="mnist") ? read_mnist_images(args.data_path, base)
                                   : read_fvecs(args.data_path, base);
    if(!ok) { std::cerr<<"Failed to load dataset\n"; return 1; }

    ok = (args.type=="mnist") ? read_mnist_images(args.query_path, queries)
                              : read_fvecs(args.query_path, queries);
    if(!ok) { std::cerr<<"Failed to load queries\n"; return 1; }

    std::cout << "Dataset: n=" << base.n << " d=" << base.d << "\n";
    std::cout << "Queries: n=" << queries.n << " d=" << queries.d << "\n";

    IVFPQ pqindex;
    pqindex.train(base, args.kclusters, args.M, args.nbits, args.seed);
    pqindex.index_dataset(base);

    int maxQ = std::min(100, (int)queries.n);

    for(int qi=0; qi<maxQ; qi++) {
        const float* qptr = queries.row(qi);

        // approximate search IVFPQ
        auto approx = pqindex.search(qptr, base, args.N, args.nprobe, args.R);

        // brute-force top-N
        auto bf_results = brute_force_search(base.to_vector(),
                                             std::vector<float>(qptr, qptr+base.d),
                                             args.N);
        std::vector<uint32_t> true_ids;
        std::vector<float> true_dists;
        for(auto &p : bf_results) {
            true_ids.push_back(p.second);
            true_dists.push_back(std::sqrt(p.first));
        }

        // range search αν R>0
        if(args.R>0) {
            auto in_range = range_search_fullscan_with_d2(base.to_vector(),
                                std::vector<float>(qptr, qptr+base.d),
                                args.R, false);
            std::cout << "R-near neighbors:\n";
            for(int id : in_range) std::cout << id << "\n";
        }

        // optional: εκτύπωση αποτελεσμάτων
        std::cout << "Query " << qi+1 << " done.\n";
    }

    return 0;
}
