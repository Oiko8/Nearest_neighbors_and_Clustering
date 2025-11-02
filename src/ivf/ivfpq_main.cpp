#include "ivf_pq.h"                       // perilamvanei ton orismo tis klasis IVFPQ
#include "../../utils_functions/io.h"      // gia diavasma datasets
#include "vectors.h"                        // Dataset struct
#include "../../utils_functions/euclid.h"  // euclidean distance
#include "../../utils_functions/nearest_neighbor.h" // nearest neighbor brute force
#include "kmeans.h"                         // kmeans training
#include "ivf_main.h"                       // klases ivf_main
#include <chrono>                           // gia chronometry
#include <algorithm>                        // std::min, std::sort
#include <iostream>                         // cout, cerr
#include <fstream>                          // ofstream
#include <cmath>                            // sqrt
#include <cstdlib>                          // exit
#include <vector>                           // vector
#include <string>                           // string

using clock_type = std::chrono::high_resolution_clock; // tipoi gia chronos
using ms = std::chrono::milliseconds;                  // tipoi gia milliseconds

// ------------------- struct gia ta command line arguments -------------------
struct Args {
    std::string method = "ivfpq";       // default method
    std::string data_path;              // -d <dataset path>
    std::string query_path;             // -q <query path>
    std::string type = "mnist";         // -type mnist|sift
    int N = 5;                          // -N number of nearest neighbors
    int kclusters = 50;                 // -kclusters
    int nprobe = 5;                     // -nprobe
    int M = 16;                          // -M subquantizers
    int nbits = 8;                        // -nbits bits per subvector
    double R = -1.0;                     // -R radius for range search
    unsigned seed = 1;                   // -seed
    bool range = false;                  // -range true|false
    std::string output_path;             // -o <output file>
};

// ------------------- parsing arguments -------------------
static Args parse_args(int argc, char** argv) {
    Args a;                              // dimiourgia struct me default times
    for (int i = 1; i < argc; ++i) {    // loop panw se ola ta args
        std::string f = argv[i];        // pairnoume to trexon arg
        auto need = [&](int more){ if (i + more >= argc) { std::cerr << "Missing value after " << f << "\n"; exit(1); } }; // elegxos an leiptei timi meta to flag

        
        if (f == "-ivfflat") { a.method = "ivfflat"; }
        else if (f == "-ivfpq") {a.method = "ivfpq" ;}
        else if (f == "-d") { need(1); a.data_path = argv[++i]; }  // -d dataset path
        else if (f == "-q") { need(1); a.query_path = argv[++i]; } // -q query path
        else if (f == "-type") { need(1); a.type = argv[++i]; }    // -type mnist|sift
        else if (f == "-N") { need(1); a.N = std::stoi(argv[++i]); } // -N number of neighbors
        else if (f == "-kclusters") { need(1); a.kclusters = std::stoi(argv[++i]); } // -kclusters
        else if (f == "-nprobe") { need(1); a.nprobe = std::stoi(argv[++i]); } // -nprobe
        else if (f == "-R") { need(1); a.R = std::stod(argv[++i]); } // -R radius
        else if (f == "-seed") { need(1); a.seed = static_cast<unsigned>(std::stoul(argv[++i])); } // -seed
        else if (f=="-M"){ need(1); a.M = std::stoi(argv[++i]); } // -M number of subquantizers
        else if (f=="-nbits"){ need(1); a.nbits = std::stoi(argv[++i]); } // -nbits bits per subvector
        else if (f == "-range") { need(1); std::string v = argv[++i]; // -range true|false
            if (v == "true" || v == "1") a.range = true; // true
            else if (v == "false" || v == "0") a.range = false; // false
            else { std::cerr << "Invalid value for -range: " << v << " (use true|false)\n"; exit(1); } // error
        }
        else if (f == "-o") { need(1); a.output_path = argv[++i]; } // -o output file
        else { std::cerr << "Unknown flag: " << f << "\n"; exit(1); } // unknown flag
    }

    if (a.method != "ivfflat" && a.method != "ivfpq") { // elegxos method
        std::cerr<<"Error: only 'ivfflat' and 'ivfpq' supported currently\n"; exit(1);
    }
    if (a.data_path.empty() || a.query_path.empty()) { // elegxos paths
        std::cerr << "Usage: ... \n"; exit(1);
    }
    return a; // epistrefei struct
}

// ------------------- main function -------------------
int ivfpq_main(int argc, char** argv) {
    Args args = parse_args(argc, argv);   // diabazei args

    Dataset base, queries;                // dimiourgia dataset kai queries
    bool ok = false;                      // flag gia epityxia diavasmatos
    std::cout << "Loading dataset: " << args.data_path << std::endl; // print
    if (args.type == "mnist") ok = read_mnist_images(args.data_path, base); // mnist
    else if (args.type == "sift") ok = read_fvecs(args.data_path, base);   // sift
    if (!ok) { std::cerr << "Error: failed to load dataset\n"; return 1; } // error check

    std::cout << "Loading queries: " << args.query_path << std::endl; // print
    if (args.type == "mnist") ok = read_mnist_images(args.query_path, queries); // mnist
    else if (args.type == "sift") ok = read_fvecs(args.query_path, queries);   // sift
    if (!ok) { std::cerr << "Error: failed to load queries\n"; return 1; } // error check

    std::cout << "Dataset vectors: n=" << base.n << "  d=" << base.d << std::endl; // info
    std::cout << "Query vectors:   n=" << queries.n << "  d=" << queries.d << std::endl; // info

    int maxQ = std::min((size_t)100, queries.n); // periorismos 100 queries
    double sum_AF = 0.0; int hits_at_N = 0;      // metrics arxikopoihsh
    double sum_tApprox_ms = 0.0, sum_tTrue_ms = 0.0; int qcount = 0; // timing arxikopoihsh

    // ------------------- open output file -------------------
    std::ostream* out = &std::cout;             // default cout
    std::ofstream fout;                          // ofstream gia arxeio
    if (!args.output_path.empty()) {             // an dwthike -o
        fout.open(args.output_path);             // anoigma arxeiou
        if (!fout.is_open()) { std::cerr << "Cannot open output file: " << args.output_path << "\n"; return 1; } // error
        out = &fout;                             // allagi stream se arxeio
    }

    // ------------------- ivfpq training -------------------
    std::cout << "Training IVFPQ with C=" << args.kclusters
              << ", M=" << args.M << ", nbits=" << args.nbits
              << ", seed=" << args.seed << " ..." << std::endl; // print training info

    IVFPQ pqindex; 
    pqindex.train(base, static_cast<size_t>(args.kclusters),
                      static_cast<size_t>(args.M),
                      static_cast<size_t>(args.nbits), args.seed); // train index
    pqindex.index_dataset(base); // index dataset

    std::cout << "IVFPQ index built. Number of lists (clusters): " << args.kclusters << std::endl; // info

    // ------------------- loop queries -------------------
    for (int qi = 0; qi < maxQ; ++qi) {
        const float* qptr = queries.row(qi); // pointer sto query dianisma

        // approximate search
        auto t0 = clock_type::now(); // start chrono
        std::vector<std::pair<uint32_t,float>> approx = pqindex.search(qptr, base, static_cast<size_t>(args.N),
                                                                       static_cast<size_t>(args.nprobe), args.R); // search
        auto t1 = clock_type::now(); // stop chrono
        double approx_ms = std::chrono::duration_cast<ms>(t1 - t0).count(); // ms

        std::vector<uint32_t> approx_ids; approx_ids.reserve(approx.size()); // ids
        std::vector<float> approx_dists;  approx_dists.reserve(approx.size()); // distances
        for (auto &p : approx) { // convert results
            approx_ids.push_back(p.first); // push id
            approx_dists.push_back(std::sqrt(p.second)); // push distance sqrt
        }

        // brute-force nearest neighbors
        t0 = clock_type::now(); // start chrono
        std::vector<uint32_t> true_ids; std::vector<float> true_dists;
        nearest_neighbor(base, qptr, args.N, true_ids, true_dists); // brute force
        t1 = clock_type::now(); // stop chrono
        double true_ms = std::chrono::duration_cast<ms>(t1 - t0).count(); // ms

        if (!approx_dists.empty() && !true_dists.empty()) { // metrics AF
            float min_approx = *std::min_element(approx_dists.begin(), approx_dists.end());
            float true_min = true_dists[0];
            sum_AF += (true_min > 0.0f) ? (min_approx / true_min) : 1.0; 
        }
        if (!true_ids.empty()) { // Recall@N
            uint32_t t0id = true_ids[0];
            for (auto id : approx_ids) { if (id == t0id) { hits_at_N++; break; } }
        }

        sum_tApprox_ms += approx_ms; // akumulation times
        sum_tTrue_ms += true_ms;
        qcount++;

        // ------------------- per query output -------------------
        (*out) << "Query: " << (qi + 1) << "\n"; // print query number
        int outN = std::min((int)approx_ids.size(), args.N); // output N
        for (int i = 0; i < outN; ++i) {
            float dA = approx_dists[i]; // approximate
            float dT = (i < (int)true_dists.size()) ? true_dists[i] : (true_dists.empty() ? 0.0f : true_dists.back()); // true distance
            (*out) << "Nearest neighbor-" << (i+1) << ": " << approx_ids[i] << "\n"; // print id
            (*out) << "distanceApproximate: " << dA << "\n"; // print approximate dist
            (*out) << "distanceTrue: " << dT << "\n"; // print true dist
        }

        // ------------------- range search -------------------
        if (args.range && args.R > 0.0) { // an oristhike range
            std::vector<int> in_range;
            auto allcand = pqindex.search(qptr, base, base.n, static_cast<size_t>(args.nprobe), args.R); // get all candidates
            for (auto &p : allcand) {
                float d = std::sqrt(p.second);
                if (d <= static_cast<float>(args.R)) in_range.push_back(static_cast<int>(p.first));
            }
            std::sort(in_range.begin(), in_range.end()); // sort
            in_range.erase(std::unique(in_range.begin(), in_range.end()), in_range.end()); // remove duplicates
            (*out) << "\nR-near neighbors:\n";
            for (int id : in_range) (*out) << id << "\n"; // print ids
        }

        (*out) << "\n";
    }

    // ------------------- summary -------------------
    double avgAF = (qcount > 0) ? (sum_AF / qcount) : 0.0; // average AF
    double recall = (qcount > 0) ? ((double)hits_at_N / qcount) : 0.0; // recall@N
    double qps = (sum_tApprox_ms > 0.0 && qcount > 0) ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0; // QPS
    double avgApproxMs = (qcount > 0) ? (sum_tApprox_ms / qcount) : 0.0; // avg approx time
    double avgTrueMs = (qcount > 0) ? (sum_tTrue_ms / qcount) : 0.0; // avg true time

    (*out) << "Average AF: " << avgAF << "\n"; // print summary
    (*out) << "Recall@N: " << recall << "\n"; 
    (*out) << "QPS: " << qps << "\n"; 
    (*out) << "tApproximateAverage: " << avgApproxMs << " ms\n"; 
    (*out) << "tTrueAverage: " << avgTrueMs << " ms\n"; 

    return 0; // telos main
}
