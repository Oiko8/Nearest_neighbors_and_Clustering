#include "ivf_flat.h"
#include "../../utils_functions/io.h"
#include "vectors.h"
#include "../../utils_functions/euclid.h"
#include "../../utils_functions/nearest_neighbor.h"
#include "kmeans.h"
#include "ivf_main.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

using clock_type = std::chrono::high_resolution_clock; // tipoi gia chronos
using ms = std::chrono::milliseconds;                  // typoi gia milliseconds

// Parsing arguments
struct Args { 
    std::string method = "ivfflat"; // default method
    std::string data_path;          // -d <path> dataset
    std::string query_path;         // -q <path> queries
    std::string type = "mnist";     // -type mnist|sift
    int N = 5;                      // -N number of neighbors
    int kclusters = 50;             // -kclusters
    int nprobe = 5;                 // -nprobe poses listes tha elegxoume
    double R = -1.0;                // -R radius (optional)
    unsigned seed = 1; 
    bool range = false; 
};

static Args parse_args(int argc, char** argv) { // function pou diabazei command line arguments
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
        else if (f == "-range") { need(1); std::string v = argv[++i]; 
            if (v == "true" || v == "1") a.range = true;
            else if (v == "false" || v == "0") a.range = false;
            else { std::cerr << "Invalid value for -range: " << v << " (use true|false)\n"; exit(1); }
        }
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

int ivfflat_main(int argc, char** argv) {
    Args args = parse_args(argc, argv);   // diabazei args

    Dataset base, queries; // dimiourgia Dataset
    bool ok = false; // flag gia success
    std::cout << "Loading dataset: " << args.data_path << std::endl; // print
    if (args.type == "mnist") ok = read_mnist_images(args.data_path, base); // mnist dataset
    else if (args.type == "sift") ok = read_fvecs(args.data_path, base);   // sift dataset
    if (!ok) { std::cerr << "Error: failed to load dataset\n"; return 1; } // error check

    std::cout << "Loading queries: " << args.query_path << std::endl; // print
    if (args.type == "mnist") ok = read_mnist_images(args.query_path, queries); // mnist queries
    else if (args.type == "sift") ok = read_fvecs(args.query_path, queries);   // sift queries
    if (!ok) { std::cerr << "Error: failed to load queries\n"; return 1; } // error check

    std::cout << "Dataset vectors: n=" << base.n << "  d=" << base.d << std::endl; // info
    std::cout << "Query vectors:   n=" << queries.n << "  d=" << queries.d << std::endl; // info

    int maxQ = std::min((size_t)100, queries.n); // prwta 100 queries
    double sum_AF = 0.0; int hits_at_N = 0; // metrics
    double sum_tApprox_ms = 0.0, sum_tTrue_ms = 0.0; int qcount = 0; // timing

    if (args.method == "ivfflat") {
        // ---------------- IVFFlat flow ----------------
        std::cout << "Training IVFFlat with C=" << args.kclusters << "  seed=" << args.seed << " ..." << std::endl;

        IVFFlat index;                                            // dhlosh objektou index
        index.train_and_index(base, static_cast<size_t>(args.kclusters), args.seed); // ekpaideush kmeans + inverted lists

        std::cout << "Index built. Number of lists (clusters): " << args.kclusters << std::endl;

        // loop panw sta queries (prwta maxQ)
        for (int qi = 0; qi < maxQ; ++qi) {
            const float* qptr = queries.row(qi);                 // pointer sto query

            // approximate search (IVFFlat)
            auto t0 = clock_type::now();
            std::vector<std::pair<uint32_t,float>> approx = index.search(qptr, base, static_cast<size_t>(args.N),
                                                                         static_cast<size_t>(args.nprobe), args.R);
            auto t1 = clock_type::now();
            double approx_ms = std::chrono::duration_cast<ms>(t1 - t0).count(); // ms

            // convert approx results to ids + actual distances (sqrt)
            std::vector<uint32_t> approx_ids; approx_ids.reserve(approx.size());
            std::vector<float> approx_dists;  approx_dists.reserve(approx.size());
            for (auto &p : approx) {
                approx_ids.push_back(p.first);
                approx_dists.push_back(std::sqrt(p.second));     // p.second = squared dist
            }

            // brute-force true top-N
            t0 = clock_type::now();
            std::vector<uint32_t> true_ids; std::vector<float> true_dists;
            nearest_neighbor(base, qptr, args.N, true_ids, true_dists);
            t1 = clock_type::now();
            double true_ms = std::chrono::duration_cast<ms>(t1 - t0).count();

            // metrics: AF + Recall@N
            if (!approx_dists.empty() && !true_dists.empty()) {
                float min_approx = *std::min_element(approx_dists.begin(), approx_dists.end());
                float true_min = true_dists[0];
                sum_AF += (true_min > 0.0f) ? (min_approx / true_min) : 1.0;
            }
            if (!true_ids.empty()) {
                uint32_t t0id = true_ids[0];
                for (auto id : approx_ids) { if (id == t0id) { hits_at_N++; break; } }
            }

            // accumulate times & counts
            sum_tApprox_ms += approx_ms;
            sum_tTrue_ms += true_ms;
            qcount++;

            // per-query output (format opws sto spec)
            std::cout << "Query: " << (qi + 1) << "\n";
            int outN = std::min((int)approx_ids.size(), args.N);
            for (int i = 0; i < outN; ++i) {
                float dA = approx_dists[i];
                float dT = (i < (int)true_dists.size()) ? true_dists[i] : (true_dists.empty() ? 0.0f : true_dists.back());
                std::cout << "Nearest neighbor-" << (i+1) << ": " << approx_ids[i] << "\n";
                std::cout << "distanceApproximate: " << dA << "\n";
                std::cout << "distanceTrue: " << dT << "\n";
            }

            // range search (an oristhike R)
            if (args.range && args.R > 0.0) {
                std::vector<int> in_range;
                auto allcand = index.search(qptr, base, base.n, static_cast<size_t>(args.nprobe), args.R);
                for (auto &p : allcand) {
                    float d = std::sqrt(p.second);
                    if (d <= static_cast<float>(args.R)) in_range.push_back(static_cast<int>(p.first));
                }
            std::sort(in_range.begin(), in_range.end());
            in_range.erase(std::unique(in_range.begin(), in_range.end()), in_range.end());
            std::cout << "\nR-near neighbors:\n";
            for (int id : in_range) std::cout << id << "\n";
            }


            std::cout << "\n";
        } // telos loop queries
    }
    // ==== final summary metrics & print ====
    double avgAF = (qcount > 0) ? (sum_AF / qcount) : 0.0;
    double recall = (qcount > 0) ? ((double)hits_at_N / qcount) : 0.0;
    double qps = (sum_tApprox_ms > 0.0 && qcount > 0) ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0;
    double avgApproxMs = (qcount > 0) ? (sum_tApprox_ms / qcount) : 0.0;
    double avgTrueMs = (qcount > 0) ? (sum_tTrue_ms / qcount) : 0.0;

    std::cout << "Average AF: " << avgAF << "\n";
    std::cout << "Recall@N: "   << recall << "\n";
    std::cout << "QPS: " << qps << "\n";
    std::cout << "tApproximateAverage: " << avgApproxMs << " ms\n";
    std::cout << "tTrueAverage: "        << avgTrueMs << " ms\n";

    return 0;
}
