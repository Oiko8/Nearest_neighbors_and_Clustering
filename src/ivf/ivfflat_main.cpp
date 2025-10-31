#include "ivf_flat.h"                               // perilamvanei ton orismo tis klasis IVFFlat
#include "../../utils_functions/io.h"               // leitourgies gia read/write dataset
#include "vectors.h"                                 // orismos tou Dataset struct
#include "../../utils_functions/euclid.h"           // eucliddistance function
#include "../../utils_functions/nearest_neighbor.h" // nearest_neighbor function prototype
#include "kmeans.h"                                  // kmeans gia training clusters
#include "ivf_main.h"                                // exoteriko header gia main ivf
#include <chrono>                                    // chrono gia chronometrisi
#include <algorithm>                                 // std::sort, std::nth_element
#include <iostream>                                  // cout, cerr
#include <fstream>                                   // ofstream gia arxeio output
#include <cmath>                                     // sqrt kai allo
#include <cstdlib>                                   // exit, atoi
#include <vector>                                    // vector
#include <string>                                    // string

using clock_type = std::chrono::high_resolution_clock; // tipoi gia chronos me high resolution
using ms = std::chrono::milliseconds;                  // milliseconds

struct Args { 
    std::string method = "ivfflat"; // default method
    std::string data_path;          // -d <path> dataset
    std::string query_path;         // -q <path> queries
    std::string type = "mnist";     // -type mnist|sift
    int N = 5;                      // -N number of neighbors
    int kclusters = 50;             // -kclusters
    int nprobe = 5;                 // -nprobe poses listes tha elegxoume
    double R = -1.0;                // -R radius (optional)
    unsigned seed = 1;              // -seed gia random generators
    bool range = false;             // -range true|false
    std::string output_path;        // -o <output file>
};

static Args parse_args(int argc, char** argv) {
    Args a; // dimiourgia struct me default values
    for (int i = 1; i < argc; ++i) { // loop panw se ola ta args
        std::string f = argv[i];      // pairnoume to trexon arg
        auto need = [&](int more){ if (i + more >= argc) { std::cerr << "Missing value after " << f << "\n"; exit(1); } }; // elegxos oti yparxei timi meta to flag

        // if (f == "-method") { need(1); a.method = argv[++i]; }       // diavase -method
        if (f == "-ivfflat") { a.method = "ivfflat"; }
        else if (f == "-ivfpq") {a.method = "ivfpq" ;}
        else if (f == "-d") { need(1); a.data_path = argv[++i]; }    // diavase -d dataset
        else if (f == "-q") { need(1); a.query_path = argv[++i]; }   // diavase -q queries
        else if (f == "-type") { need(1); a.type = argv[++i]; }      // diavase -type mnist|sift
        else if (f == "-N") { need(1); a.N = std::stoi(argv[++i]); } // diavase -N number of neighbors
        else if (f == "-kclusters") { need(1); a.kclusters = std::stoi(argv[++i]); } // -kclusters
        else if (f == "-nprobe") { need(1); a.nprobe = std::stoi(argv[++i]); } // -nprobe
        else if (f == "-R") { need(1); a.R = std::stod(argv[++i]); }  // -R radius
        else if (f == "-seed") { need(1); a.seed = static_cast<unsigned>(std::stoul(argv[++i])); } // -seed
        else if (f == "-range") { need(1); std::string v = argv[++i]; 
            if (v == "true" || v == "1") a.range = true;              // range = true
            else if (v == "false" || v == "0") a.range = false;       // range = false
            else { std::cerr << "Invalid value for -range: " << v << " (use true|false)\n"; exit(1); }
        }
        else if (f == "-o") { need(1); a.output_path = argv[++i]; }   // -o output file
        else { std::cerr << "Unknown flag: " << f << "\n"; exit(1); } // unknown flag
    }
    if (a.method != "ivfflat" && a.method != "ivfpq") { // elegxos method
        std::cerr<<"Error: only 'ivfflat' and 'ivfpq' supported currently\n";
        exit(1);
    }
    if (a.data_path.empty() || a.query_path.empty()) { // elegxos paths
        std::cerr << "Usage: ... \n"; exit(1);
    }
    return a; // epistrefei struct me args
}

int ivfflat_main(int argc, char** argv) {
    Args args = parse_args(argc, argv);   // diabazei arguments

    std::ostream* out = &std::cout;       // default output = console
    std::ofstream fout;                    // gia arxeio output
    if (!args.output_path.empty()) {       // an dilwthike -o
        fout.open(args.output_path);       // anoigma arxeiou
        if (!fout.is_open()) { std::cerr << "Cannot open output file: " << args.output_path << "\n"; return 1; }
        out = &fout;                       // allagi pointer se arxeio
    }

    Dataset base, queries;                 // datasets base kai queries
    bool ok = false; 
    (*out) << "Loading dataset: " << args.data_path << std::endl; 
    if (args.type == "mnist") ok = read_mnist_images(args.data_path, base); // an mnist dataset
    else if (args.type == "sift") ok = read_fvecs(args.data_path, base);    // an sift dataset
    if (!ok) { std::cerr << "Error: failed to load dataset\n"; return 1; } 

    (*out) << "Loading queries: " << args.query_path << std::endl; 
    if (args.type == "mnist") ok = read_mnist_images(args.query_path, queries); 
    else if (args.type == "sift") ok = read_fvecs(args.query_path, queries);   
    if (!ok) { std::cerr << "Error: failed to load queries\n"; return 1; } 

    (*out) << "Dataset vectors: n=" << base.n << "  d=" << base.d << std::endl; 
    (*out) << "Query vectors:   n=" << queries.n << "  d=" << queries.d << std::endl; 

    int maxQ = std::min((size_t)100, queries.n);       // max 100 queries
    double sum_AF = 0.0; int hits_at_N = 0;           // metrics
    double sum_tApprox_ms = 0.0, sum_tTrue_ms = 0.0; int qcount = 0; // timing
    if (args.method == "ivfflat") {                                 // an to method einai ivfflat
    (*out) << "Training IVFFlat with C=" << args.kclusters << "  seed=" << args.seed << " ..." << std::endl; // emfanisi info

    IVFFlat index;                                               // dhmiourgia antikeimenou index IVFFlat
    index.train_and_index(base, static_cast<size_t>(args.kclusters), args.seed); // ekpaideush + index

    (*out) << "Index built. Number of lists (clusters): " << args.kclusters << std::endl; // emfanisi clusters

    for (int qi = 0; qi < maxQ; ++qi) {                        // loop panw se queries
        const float* qptr = queries.row(qi);                   // pointer sto query dianisma

        auto t0 = clock_type::now();                           // arxikos xronos
        std::vector<std::pair<uint32_t,float>> approx = index.search(qptr, base, static_cast<size_t>(args.N),
                                                                     static_cast<size_t>(args.nprobe), args.R); // approximate search
        auto t1 = clock_type::now();                           // telikos xronos
        double approx_ms = std::chrono::duration_cast<ms>(t1 - t0).count(); // xronos se ms

        std::vector<uint32_t> approx_ids; approx_ids.reserve(approx.size()); // ids twn approximate apotelesmatwn
        std::vector<float> approx_dists; approx_dists.reserve(approx.size()); // apostaseis twn approximate apotelesmatwn
        for (auto &p : approx) {                               // loop panw sta results
            approx_ids.push_back(p.first);                     // prosthese id
            approx_dists.push_back(std::sqrt(p.second));      // prosthese apostasi (sqrt apo squared)
        }

        t0 = clock_type::now();                                // arxikos xronos true search
        std::vector<uint32_t> true_ids; std::vector<float> true_dists; // containers gia true nearest
        nearest_neighbor(base, qptr, args.N, true_ids, true_dists); // brute-force true top-N
        t1 = clock_type::now();                                // telikos xronos
        double true_ms = std::chrono::duration_cast<ms>(t1 - t0).count(); // xronos se ms

        if (!approx_dists.empty() && !true_dists.empty()) {    // elegxos na min einai empty
            float min_approx = *std::min_element(approx_dists.begin(), approx_dists.end()); // elaxisti aprox apostasi
            float true_min = true_dists[0];                   // mikroteri true apostasi
            sum_AF += (true_min > 0.0f) ? (min_approx / true_min) : 1.0; // upologismos AF
        }
        if (!true_ids.empty()) {                               // elegxos an true_ids den einai empty
            uint32_t t0id = true_ids[0];                       // prwto true id
            for (auto id : approx_ids) {                       // loop panw sta approx ids
                if (id == t0id) { hits_at_N++; break; }       // an breTthei prwto true id, increment hits
            }
        }

        sum_tApprox_ms += approx_ms;                           // akolouthisi xronou approximate
        sum_tTrue_ms += true_ms;                               // akolouthisi xronou true
        qcount++;                                              // metritis queries

        (*out) << "Query: " << (qi + 1) << "\n";              // emfanisi query number
        int outN = std::min((int)approx_ids.size(), args.N);  // emfanizomena apotelesmata
        for (int i = 0; i < outN; ++i) {                      
            float dA = approx_dists[i];                       
            float dT = (i < (int)true_dists.size()) ? true_dists[i] : (true_dists.empty() ? 0.0f : true_dists.back()); // distance true
            (*out) << "Nearest neighbor-" << (i+1) << ": " << approx_ids[i] << "\n"; // emfanisi id
            (*out) << "distanceApproximate: " << dA << "\n";   // emfanisi aprox distance
            (*out) << "distanceTrue: " << dT << "\n";          // emfanisi true distance
        }

        if (args.range && args.R > 0.0) {                       // an exoume range search
            std::vector<int> in_range;                          // container gia R-near ids
            auto allcand = index.search(qptr, base, base.n, static_cast<size_t>(args.nprobe), args.R); // ola ta candidates
            for (auto &p : allcand) {                           
                float d = std::sqrt(p.second);                  // apostasi
                if (d <= static_cast<float>(args.R)) in_range.push_back(static_cast<int>(p.first)); // prosthese an mesa sto R
            }
            std::sort(in_range.begin(), in_range.end());        // sort ids
            in_range.erase(std::unique(in_range.begin(), in_range.end()), in_range.end()); // remove duplicates
            (*out) << "\nR-near neighbors:\n";                 // emfanisi header
            for (int id : in_range) (*out) << id << "\n";      // emfanisi ids
        }

        (*out) << "\n";                                         // empty line
    }
}

double avgAF = (qcount > 0) ? (sum_AF / qcount) : 0.0;       // average AF
double recall = (qcount > 0) ? ((double)hits_at_N / qcount) : 0.0; // recall@N
double qps = (sum_tApprox_ms > 0.0 && qcount > 0) ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0; // queries/sec
double avgApproxMs = (qcount > 0) ? (sum_tApprox_ms / qcount) : 0.0; // avg approximate ms
double avgTrueMs = (qcount > 0) ? (sum_tTrue_ms / qcount) : 0.0;     // avg true ms

(*out) << "Average AF: " << avgAF << "\n";                   // emfanisi AF
(*out) << "Recall@N: "   << recall << "\n";                 // emfanisi recall
(*out) << "QPS: " << qps << "\n";                            // emfanisi QPS
(*out) << "tApproximateAverage: " << avgApproxMs << " ms\n"; // emfanisi tApprox
(*out) << "tTrueAverage: "        << avgTrueMs << " ms\n";   // emfanisi tTrue

return 0;                                                     // telos main function
}