// ---------- header includes ----------
#include "ivf_flat.h"    // xrhsimopoioume ton IVFFlat index (train & search)
#include "io.h"          // read_mnist_images / read_fvecs -> gemizoume Dataset (float)
#include "vectors.h"     // Dataset struct me continuous memory (row-major)
#include "euclid.h"      // eucliddistance(a,b,d) -> epistrefei squared L2 (float)
#include <chrono>        // gia timing
#include <algorithm>     // nth_element, sort, min_element
#include <iostream>      // cout/cerr
#include <cmath>         // sqrt
#include <cstdlib>       // exit

// ===== parse args struct =====
struct Args {
    std::string method = "ivfflat"; // default, dinoume -method ivfflat
    std::string data_path;          // -d <path> (dataset)
    std::string query_path;         // -q <path> (queries)
    std::string type = "mnist";     // -type mnist|sift
    int N = 5;                      // -N number of neighbors pou theloume
    int C = 50;                    // -C clusters (kmeans clusters)
    int nprobe = 5;                 // -nprobe poses listes tha anazhtisoume
    double R = -1.0;                // -R radius (optional)
    unsigned seed = 1;              // -seed gia kmeans
};

// ===== parse_args() =====
Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string f = argv[i];
        auto need = [&](int more){ if (i + more >= argc) { std::cerr << "Missing value after " << f << "\n"; exit(1); } };
        if (f == "-method") { need(1); a.method = argv[++i]; }       // diavase -method
        else if (f == "-d") { need(1); a.data_path = argv[++i]; }    // diavase -d dataset path
        else if (f == "-q") { need(1); a.query_path = argv[++i]; }   // diavase -q queries path
        else if (f == "-type") { need(1); a.type = argv[++i]; }      // diavase -type (mnist/sift)
        else if (f == "-N") { need(1); a.N = std::stoi(argv[++i]); } // -N number of neighbors
        else if (f == "-C") { need(1); a.C = std::stoi(argv[++i]); }// -C clusters
        else if (f == "-nprobe") { need(1); a.nprobe = std::stoi(argv[++i]); } // -nprobe
        else if (f == "-R") { need(1); a.R = std::stod(argv[++i]); }  // -R radius
        else if (f == "-seed") { need(1); a.seed = static_cast<unsigned>(std::stoul(argv[++i])); } // -seed
        else { std::cerr << "Unknown flag: " << f << "\n"; exit(1); }
    }
    if (a.method != "ivfflat") {
        std::cerr << "Error: mono 'ivfflat' ypostirizetai se auto to executable (use -method ivfflat)\n";
        exit(1);
    }
    if (a.data_path.empty() || a.query_path.empty()) {
        std::cerr << "Usage: ... \n"; exit(1);
    }
    // leave R unset ean den oristhike: a.R stays -1.0 (den kanoume default range)
    return a;
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

// ===== main =====
int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);   // diabaze ta args / mpainoun se struct

    // fortwma dataset kai queries se Dataset (float)
    Dataset base, queries;
    bool ok = false;
    std::cout << "Loading dataset: " << args.data_path << std::endl;
    if (args.type == "mnist") ok = read_mnist_images(args.data_path, base);
    else if (args.type == "sift") ok = read_fvecs(args.data_path, base);
    if (!ok) { std::cerr << "Error: failed to load dataset\n"; return 1; }

    std::cout << "Loading queries: " << args.query_path << std::endl;
    if (args.type == "mnist") ok = read_mnist_images(args.query_path, queries);
    else if (args.type == "sift") ok = read_fvecs(args.query_path, queries);
    if (!ok) { std::cerr << "Error: failed to load queries\n"; return 1; }

    std::cout << "Dataset vectors: n=" << base.n << "  d=" << base.d << std::endl;
    std::cout << "Query vectors:   n=" << queries.n << "  d=" << queries.d << std::endl;

    // train IVFFlat
    std::cout << "Training IVFFlat with C=" << args.C << "  seed=" << args.seed << " ...\n";
    IVFFlat index;
    index.train_and_index(base, static_cast<size_t>(args.C), args.seed);
    std::cout << "Index built. Number of lists (clusters): " << args.C << "\n";

    // initialize metrics
    int maxQ = std::min((size_t)100, queries.n); // teleutaia: kai epilogh gia #queries
    double sum_AF = 0.0;
    int hits_at_N = 0;
    double sum_tApprox_ms = 0.0;
    double sum_tTrue_ms = 0.0;
    int qcount = 0;

    // loop panw se queries (prwta 100)
    for (int qi = 0; qi < maxQ; ++qi) {
        const float* qptr = queries.row(qi); // pairnoume pointer sto query

        // ---------- approximate search (IVFFlat) ----------
        auto t0 = clock_type::now();
        // index.search(q, dataset, N, nprobe, R) : epistrefei vector<pair<id, distSquared>>
        std::vector<std::pair<uint32_t,float>> approx = index.search(qptr, base, static_cast<size_t>(args.N), static_cast<size_t>(args.nprobe), args.R);
        auto t1 = clock_type::now();
        double approx_ms = std::chrono::duration_cast<ms>(t1 - t0).count();

        // kataxwrisi approx ids kai distances
        std::vector<uint32_t> approx_ids;
        std::vector<float> approx_dists;
        approx_ids.reserve(approx.size());
        approx_dists.reserve(approx.size());
        for (auto &p : approx) {
            approx_ids.push_back(p.first);              // p.first = id tou vector mesa sto dataset
            approx_dists.push_back(std::sqrt(p.second));// p.second = squared dist -> sqrt gia real L2
        }

        // ---------- true brute-force ----------
        t0 = clock_type::now();
        std::vector<uint32_t> true_ids;
        std::vector<float> true_dists;
        nearest_neighbor(base, qptr, args.N, true_ids, true_dists); // apothikeuei true top-N
        t1 = clock_type::now();
        double true_ms = std::chrono::duration_cast<ms>(t1 - t0).count();

        // ---------- metrics ----------
        if (!approx_dists.empty() && !true_dists.empty()) {
            float min_approx = *std::min_element(approx_dists.begin(), approx_dists.end()); // mikroteri apo approx
            float true_min = true_dists[0]; // true top-1 distance
            if (true_min > 0.0f) sum_AF += (double)(min_approx / true_min); // AF = approx_min / true_min
            else sum_AF += 1.0;
        }
        if (!true_ids.empty()) {
            uint32_t true0 = true_ids[0]; // id tou true nearest
            for (auto id : approx_ids) { if (id == true0) { hits_at_N++; break; } } // Recall@N
        }

        sum_tApprox_ms += approx_ms;
        sum_tTrue_ms += true_ms;
        qcount++;

        // ---------- print results per query ----------
        std::cout << "Query: " << (qi + 1) << "\n";
        int outN = std::min((int)approx_ids.size(), args.N);
        for (int i = 0; i < outN; ++i) {
            float dA = approx_dists[i];
            float dT = (i < (int)true_dists.size()) ? true_dists[i] : (true_dists.empty() ? 0.0f : true_dists.back());
            std::cout << "Nearest neighbor-" << (i+1) << ": " << approx_ids[i] << "\n";
            std::cout << "distanceApproximate: " << dA << "\n";
            std::cout << "distanceTrue: " << dT << "\n";
        }

        // ---------- range-search (an oristhike R) ----------
        if (args.R > 0.0) {
            std::vector<int> in_range;
            auto allcand = index.search(qptr, base, base.n, static_cast<size_t>(args.nprobe), args.R);
            for (auto &p : allcand) {
                float d = std::sqrt(p.second);
                if (d <= (float)args.R) in_range.push_back((int)p.first);
            }
            std::sort(in_range.begin(), in_range.end());
            in_range.erase(std::unique(in_range.begin(), in_range.end()), in_range.end());
            std::cout << "\nR-near neighbors:\n";
            for (int id : in_range) std::cout << id << "\n";
        }

        std::cout << "\n";
    } // telos loop queries

    // ===== summary =====
    double avgAF = (qcount > 0) ? (sum_AF / qcount) : 0.0;
    double recall = (qcount > 0) ? ((double)hits_at_N / qcount) : 0.0;
    double qps = (sum_tApprox_ms > 0.0 && qcount>0) ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0;
    double avgApproxMs = (qcount > 0) ? (sum_tApprox_ms / qcount) : 0.0;
    double avgTrueMs   = (qcount > 0) ? (sum_tTrue_ms / qcount) : 0.0;

    std::cout << "Average AF: " << avgAF << "\n";
    std::cout << "Recall@N: "   << recall << "\n";
    std::cout << "QPS: " << qps << "\n";
    std::cout << "tApproximateAverage: " << avgApproxMs << " ms\n";
    std::cout << "tTrueAverage: "        << avgTrueMs   << " ms\n";

    return 0;
}
