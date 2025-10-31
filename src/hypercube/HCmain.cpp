// Usage : 
// MNIST : ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 10 -w 10.0 -M 20 -probes 8 -N 4 -R 5.0 -type mnist -range false
#include "Hypercube.h"
#include "../../utils_functions/Data_loader.h"
#include "../../utils_functions/euclid.h"
#include <fstream>

using clock_type = std::chrono::steady_clock;
using ms = std::chrono::duration<float, std::milli>;

struct Args {
    string data_path;         // -d
    string query_path;        // -q (optional)
    string type = "mnist";    // -type mnist|sift
    string algorithm;         // which algorithm we use
    string output_file = "output.txt";  // -o
    int k = 1;                // -N
    int kproj = 14;           // -kproj
    int M = 5;                // -M (per-vertex or total depending on config)
    int probes = 4;           // -probes (number of vertices to visit)
    int queries_num = 0;      // subset of queries you choose to run 
    float w = 4.0;            // -w
    float R = 2000.0;         // -R (MNIST default)
    bool range = false;       // -range (if there will be a range search)
    bool norm = true;
};

static bool str2bool(const string& s){
    return (s=="1"||s=="true"||s=="True"||s=="TRUE");
}

static Args parse_args(int argc, char** argv){
    Args a;
    for (int i=1; i<argc; ++i){
        string flag = argv[i];
        auto need = [&](int more){ if(i+more>=argc) { cerr<<"Missing value after "<<flag<<"\n"; exit(1);} };
        if (flag=="-d") { need(1); a.data_path=argv[++i]; }
        else if (flag=="-q") { need(1); a.query_path=argv[++i]; }
        else if (flag=="-type"){ need(1); a.type=argv[++i]; }
        else if (flag=="-o") { need(1); a.output_file=argv[++i]; }
        else if (flag=="-N") { need(1); a.k=stoi(argv[++i]); }
        else if (flag=="-kproj") { need(1); a.kproj=stoi(argv[++i]); }
        else if (flag=="-M") { need(1); a.M=stoi(argv[++i]); }
        else if (flag=="-probes") { need(1); a.probes=stoi(argv[++i]); }
        else if (flag=="-w") { need(1); a.w=stod(argv[++i]); }
        else if (flag=="-R") { need(1); a.R=stod(argv[++i]); }
        else if (flag=="-range"){ need(1); a.range=str2bool(argv[++i]); }
        else if (flag=="-hypercube"){  a.algorithm="hypercube"; }
        else if (flag=="-raw"){ a.norm=false; } 
        else if (flag=="-subset"){ a.queries_num = stoi(argv[++i]); }
        else { cerr<<"Unknown flag: "<<flag<<"\n"; exit(1);}
    }
    if (a.data_path.empty()){
        cerr<<"Usage: "<<argv[0]<<" -d <input.dat> [-q <query.dat>] [-type mnist|sift] "
              " -kproj <kproj> -w <w> -M <M> -probes <probes> -N <N> -R <R> -range <true|false> -hypercube\n";
        exit(1);
    }
    
    return a;
}

static void search_in_dataset(Args args , string type){

    ofstream out(args.output_file);
    if (!out.is_open()) {
        cerr << "Error: could not open output file " << args.output_file << endl;
        exit(1);
    }

    vector<vector<float>> pts;
    vector<vector<float>> queries;
    if (type == "mnist"){
        // From the input file extract the data and transform the correct way
        string input_file = args.data_path;
        cout << "Loading dataset: " << input_file << endl;
        pts = load_mnist_dataset(input_file);
        cout << "Loaded " << pts.size() << " images of dimension " << (pts.empty()?0:pts[0].size()) << endl;
    
        cout << "*************************************************\n";
    
        // From the query file extract the data and transform the correct way
        string query_file = args.query_path;
        cout << "Loading queries: " << query_file << endl;
        queries = load_mnist_dataset(query_file);
        cout << "Loaded " << queries.size() << " test images of dimension " << (queries.empty()?0:queries[0].size()) << endl;
    
        // Normalize the vectors: 0-255 --> 0-1
        // find max number of the set
        float max = pts[0][0];
        for (auto p : pts) {
            for (auto v : p) {
                if (v > max) max = v;
            }
        }
        if (args.norm == true) {
            for (auto &point : pts){
                for (float &dim : point) {
                    dim /= max;
                }
            }
            for (auto &query : queries){
                for (float &dim : query) {
                    dim /= max;
                }
            }
        }
    }
    else if (type == "sift") {
        // From the input file extract the data and transform the correct way
        string input_file = args.data_path;
        cout << "Loading dataset: " << input_file << endl;
        pts = load_sift_dataset(input_file);
        cout << "Loaded " << pts.size() << " images of dimension " << (pts.empty()?0:pts[0].size()) << endl;

        cout << "*************************************************\n";

        // From the query file extract the data and transform the correct way
        string query_file = args.query_path;
        cout << "Loading queries: " << query_file << endl;
        queries = load_sift_dataset(query_file);
        cout << "Loaded " << queries.size() << " test images of dimension " << (queries.empty()?0:queries[0].size()) << endl;

        // Normalize the vectors: --> 0-1
        // find max number of the set
        float max = pts[0][0];
        for (auto p : pts) {
            for (auto v : p) {
                if (v > max) max = v;
            }
        }
        if (args.norm == true) {
            for (auto &point : pts){
                for (float &dim : point) {
                    dim /= max;
                }
            }
            for (auto &query : queries){
                for (float &dim : query) {
                    dim /= max;
                }
            }
        }
    }

    int kproj = args.kproj, N = args.k;
    int M = args.M;
    int probes = args.probes;
    float w = args.w;
    
    // Build hypercube index
    build_hypercube(pts, kproj, w);

    vector<float> q;

    float sum_AF = 0.0;
    int qcount = 0;
    int hits_at_N = 0;                 // Recall@N counter
    float sum_recall = 0.0;           // to calculate the average recall
    float sum_tApprox_ms = 0.0;       // sum of the time for the approx search
    float sum_tTrue_ms   = 0.0;       // sum of the time for the true search 
 
    // if a subset is not provided take all the set of queries
    int queries_num = (args.queries_num==0) ? (int)queries.size() : args.queries_num;

    out << "[Hypercube]\n\n";

    for (int i=0 ; i<queries_num && i < (int)queries.size(); i++){
        q = queries[i];

        // =====================================================================
        // ============================ NN Search===============================
        // =====================================================================

        // ============= Approximate search (Hypercube) and the time needed ==========
        auto t0 = clock_type::now();
        vector<int> nn_idx = cube_query_knn(pts, q, N, M, probes);
        auto t1 = clock_type::now();
        float approx_search_time = chrono::duration_cast<ms>(t1 - t0).count();

        // Compute distances for the approx list (in the same order)
        vector<float> approx_dists; approx_dists.reserve(nn_idx.size());
        for (int id : nn_idx) {
            approx_dists.push_back(euclidean_distance(pts[id], q));
        }

        
        // ============= Search with brute force and the time needed ===========
        t0 = clock_type::now();
        vector<pair<float,int>> all;
        all.reserve(pts.size());
        double dist = 0.0;
        for (int i = 0 ; i < static_cast<int>(pts.size()) ; i++) {
            dist = euclidean_distance(pts[i], q);
            all.emplace_back(dist, i);
        }

        // use nth_element() and after sort only the n smaller distances
        nth_element(all.begin(), all.begin() + N, all.end(),
                    [](auto& a, auto& b){ return a.first < b.first; });
        // keeping only the first N elements, that are the N smallest values
        all.resize(N);

        sort(all.begin(), all.end());
        t1 = clock_type::now();
        float true_search_time = chrono::duration_cast<ms>(t1 - t0).count();

        vector<int> true_topN_ids;
        true_topN_ids.reserve(all.size());
        vector<float> true_topN_dists;
        true_topN_dists.reserve(all.size());
        for (auto& p : all) { 
            true_topN_dists.push_back(p.first);
            true_topN_ids.push_back(p.second); 
        }

        // =====================================================================
        // ========================= Range Search ==============================
        // =====================================================================
        vector<int> in_range_idx;
        if (args.range == true){
            float R = args.R;
            in_range_idx = cube_range_search(pts, q, R, M, probes); 
            // in_range_idx = range_search_fullscan(pts, q, R);        
        }



        // =====================================================================
        // ========================= Prepare the metrics =======================
        // =====================================================================
        
        // ========================= Recall @N =================================
        // the average fraction of true_nearest_neighbor/approx_nearest neigbors.
        hits_at_N = 0;
        for (int true_nn_id : true_topN_ids){
            for (int approx_id : nn_idx) {
                if (true_nn_id == approx_id) {
                    hits_at_N ++;
                }
            }
        }

        float recall = static_cast<float>(hits_at_N) / max(1, N);
        sum_recall += recall;


        // ======================= Approximation Fraction =====================
        float min_approx_dist = std::numeric_limits<float>::infinity();
        if (!approx_dists.empty()) min_approx_dist = *min_element(approx_dists.begin(), approx_dists.end());
        float AF = (true_topN_dists.empty() || true_topN_dists[0] == 0.0f) ? 0.0f : (min_approx_dist / true_topN_dists[0]);
        sum_AF += AF;
        sum_tApprox_ms += approx_search_time;
        sum_tTrue_ms   += true_search_time;
        qcount++;
        


        
        // =====================================================================
        // ============================== Results ==============================
        // =====================================================================
        out << "Query: " << i+1 << "\n";
        for (int i = 0; i < (int)nn_idx.size(); ++i) {
            out << "Nearest neighbor-" << (i+1) << ": " << nn_idx[i] << "\n";
            out << "distanceApproximate: " << approx_dists[i] << "\n";
            // If we have fewer true items than i+1 (shouldn’t happen), clamp
            float dtrue_i = true_topN_dists[ min(i, (int)true_topN_dists.size()-1) ];
            out << "distanceTrue: " << dtrue_i << "\n";
        }
        
        if (args.range == true) {
            out << "\nR-near neighbors:\n";
            for (int id : in_range_idx) out << id << "\n";
        }
        out << "\n";
    }
    // =========================================================================
    // ========================== Summary ======================================
    // =========================================================================

    out << "Average AF: " << (qcount ? (sum_AF / qcount) : 0.0) << "\n";
    out << "Recall@N: "   << (qcount ? sum_recall / qcount : 0.0) << "\n";

    float tApproxAvg = qcount ? (sum_tApprox_ms / qcount) : 0.0;
    float tTrueAvg   = qcount ? (sum_tTrue_ms   / qcount) : 0.0;
    out << "QPS: " << (sum_tApprox_ms > 0.0 ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0) << "\n";
    out << "tApproximateAverage: " << tApproxAvg << "\n";
    out << "tTrueAverage: "        << tTrueAvg   << "\n";

    // print for testing
    cout << "Average AF: " << (qcount ? (sum_AF / qcount) : 0.0) << "\n";
    cout << "Recall@N: "   << (qcount ? sum_recall / qcount : 0.0) << "\n";

    cout << "QPS: " << (sum_tApprox_ms > 0.0 ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0) << "\n";
    cout << "tApproximateAverage: " << tApproxAvg << "\n";
    cout << "tTrueAverage: "        << tTrueAvg   << "\n";
}


//$./main -d <input file> -q <query file> -kproj <int> -w <float> -M <int> -probes <int> -N <number of nearest> -R <radius> 
int hypercube_main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " -d <input file> -q <query file> -kproj <int> -w <float>" 
                                     << " -M <per-vertex> -probes <int> -N <number of nearest> -R <radius> " << endl;
        return 1;
    }
    Args args = parse_args(argc, argv);

    string type = args.type;
    search_in_dataset(args, type);
    return 0;
}