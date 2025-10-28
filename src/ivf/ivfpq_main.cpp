#include "ivf_pq.h"
#include "../../utils_functions/io.h"
#include "vectors.h"
#include "../../utils_functions/euclid.h"
#include "kmeans.h"
#include "ivf_main.h"
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
// ===== nearest_neighbor =====
// pair(distSquared, id) einai plhsios tropos gia nth_element
static void nearest_neighbor(const Dataset &ds, const float* q, int N,
                                      std::vector<uint32_t> &out_ids, std::vector<float> &out_dists) {
    std::vector<std::pair<float,uint32_t>> all;   // vector apo (distSquared, id)
    all.reserve(ds.n);                            // kratame capacity gia apodosh

    for (size_t i=0;i<ds.n;i++){
        float dsq = eucliddistance(q, ds.row(i), ds.d);
        // emplace_back: prosthikei to \pair(dsq, i) sto telos tou vector, xwris extra copy
        all.emplace_back(dsq, (uint32_t)i);
    }

    // an exoume perissotera apo N, xrisimopoioume nth_element gia na pairnoume top-N stoixia
    if ((int)all.size() > N) {
        std::nth_element(all.begin(), all.begin() + N, all.end(),
                         [](const auto &a, const auto &b){ return a.first < b.first; });
        // resize gia na kratame mono ta prwta N stoixeia (ta opoia einai ta N mikrotera, alla oxi sorted)
        all.resize(N);
    }

    // twra ta sortaroume teleia (smallest -> largest)
    std::sort(all.begin(), all.end(), [](const auto &a, const auto &b){ return a.first < b.first; });

    // clear out vectors and gemise ta out_ids,out_dists me apotelesmata
    out_ids.clear(); out_dists.clear();
    for (auto &p : all) {
        out_ids.push_back(p.second);           // p.second einai to id (uint32)
        out_dists.push_back(std::sqrt(p.first)); // p.first einai squared dist, opote sqrt-> actual L2
    }
}
int ivfpq_main(int argc, char** argv) {
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

    // ======= method dispatch: ivfflat OR ivfpq =======
    if (args.method == "ivfpq") {
        std::cout << "Training IVFPQ with C=" << args.kclusters
                  << ", M=" << args.M << ", nbits=" << args.nbits
                  << ", seed=" << args.seed << " ..." << std::endl;

        IVFPQ pqindex; // IVFPQ objekt
        pqindex.train(base, static_cast<size_t>(args.kclusters),
                          static_cast<size_t>(args.M),
                          static_cast<size_t>(args.nbits), args.seed); // train: ivf centroids & pq codebooks
        pqindex.index_dataset(base); // index dataset: kodikopoihsh kai poulema se inverted lists

        std::cout << "IVFPQ index built. Number of lists (clusters): " << args.kclusters << std::endl;

        // loop panw sta queries (prwta maxQ)
        for (int qi = 0; qi < maxQ; ++qi) {
            const float* qptr = queries.row(qi); // pointer sto query

            // approximate search me IVFPQ
            auto t0 = clock_type::now();
            std::vector<std::pair<uint32_t,float>> approx = pqindex.search(qptr, base, static_cast<size_t>(args.N),
                                                                           static_cast<size_t>(args.nprobe), args.R);
            auto t1 = clock_type::now();
            double approx_ms = std::chrono::duration_cast<ms>(t1 - t0).count();

            // convert approx results (ids + actual distances)
            std::vector<uint32_t> approx_ids; approx_ids.reserve(approx.size());
            std::vector<float> approx_dists;  approx_dists.reserve(approx.size());
            for (auto &p : approx) {
                approx_ids.push_back(p.first);
                approx_dists.push_back(std::sqrt(p.second)); // p.second = squared approx dist
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

            // per-query output
            std::cout << "Query: " << (qi + 1) << "\n";
            int outN = std::min((int)approx_ids.size(), args.N);
            for (int i = 0; i < outN; ++i) {
                float dA = approx_dists[i];
                float dT = (i < (int)true_dists.size()) ? true_dists[i] : (true_dists.empty() ? 0.0f : true_dists.back());
                std::cout << "Nearest neighbor-" << (i+1) << ": " << approx_ids[i] << "\n";
                std::cout << "distanceApproximate: " << dA << "\n";
                std::cout << "distanceTrue: " << dT << "\n";
            }

            // range search (an oristhike)
            if (args.R > 0.0) {
                std::vector<int> in_range;
                auto allcand = pqindex.search(qptr, base, base.n, static_cast<size_t>(args.nprobe), args.R);
                for (auto &p : allcand) { float d = std::sqrt(p.second); if (d <= (float)args.R) in_range.push_back((int)p.first); }
                std::sort(in_range.begin(), in_range.end());
                in_range.erase(std::unique(in_range.begin(), in_range.end()), in_range.end());
                std::cout << "\nR-near neighbors:\n";
                for (int id : in_range) std::cout << id << "\n";
            }

            std::cout << "\n";
        } // telos queries loop
    }
    else {
        std::cerr << "Unknown method: " << args.method << "\n";
        return 1;
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