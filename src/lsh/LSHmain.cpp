// Usage : 
// MNIST : ./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 12 -w 6.0 -N 5 -R 5 -type mnist -range true
// SIFT  : ./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 8 -w 250.0 -N 5 -R 200 -type sift -range true

#include "Euclidean_Hashing.h"
#include "../../utils_functions/Data_loader.h"
#include "../bruteforce/BruteForceImplementation.h"
#include "../../utils_functions/euclid.h"
#include "../../utils_functions/Rangesearch.h"
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
    int L = 5;                // -L
    int khash = 4;            // -k
    float w = 4.0;           // -w
    float R = 2000.0;        // -R (MNIST default)
    bool range = false;       // -range (if there will be a range search)
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
        else if (flag=="-L") { need(1); a.L=stoi(argv[++i]); }
        else if (flag=="-k") { need(1); a.khash=stoi(argv[++i]); }
        else if (flag=="-w") { need(1); a.w=stod(argv[++i]); }
        else if (flag=="-R") { need(1); a.R=stod(argv[++i]); }
        else if (flag=="-range"){ need(1); a.range=str2bool(argv[++i]); }
        else if (flag=="-lsh"){  a.algorithm="lsh"; }
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
        cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;
    
        cout << "*************************************************\n";
    
        // From the query file extract the data and transform the correct way
        string query_file = args.query_path;
        cout << "Loading queries: " << query_file << endl;
        queries = load_mnist_dataset(query_file);
        cout << "Loaded " << queries.size() << " test images of dimension " << queries[0].size() << endl;
    
        // Normalize the vectors: 0-255 --> 0-1
        for (auto &point : pts){
            for (float &dim : point) {
                dim /= 255.0;
            }
        }
        for (auto &query : queries){
            for (float &dim : query) {
                dim /= 255.0;
            }
        }
    }
    else if (type == "sift") {
        // From the input file extract the data and transform the correct way
        string input_file = args.data_path;
        cout << "Loading dataset: " << input_file << endl;
        pts = load_sift_dataset(input_file);
        cout << "Loaded " << pts.size() << " images of dimension " << pts[0].size() << endl;

        cout << "*************************************************\n";

        // From the query file extract the data and transform the correct way
        string query_file = args.query_path;
        cout << "Loading queries: " << query_file << endl;
        queries = load_sift_dataset(query_file);
        cout << "Loaded " << queries.size() << " test images of dimension " << queries[0].size() << endl;
    }

    int L = args.L, khash = args.khash, N = args.k;
    float w = args.w;
    
    // Prepare the tables according to the data
    build_hash_tables(pts, L, khash, w);


    vector<float> q;

    float sum_AF = 0.0;
    int qcount = 0;
    int hits_at_N = 0;                 // Recall@N counter
    float sum_recall = 0.0;           // to calculate the average recall
    float sum_tApprox_ms = 0.0;       // sum of the time for the approx search
    float sum_tTrue_ms   = 0.0;       // sum of the time for the true search 
 
    int queries_num = min(100, (int)queries.size());
    // int queries_num = (int)queries.size();
    
    for (int i=0 ; i < queries_num; i++){
        q = queries[i];


        // =====================================================================
        // ============================ NN Search===============================
        // =====================================================================

        // ============= Approximate search (LSH) and the time needed ==========
        auto t0 = clock_type::now();
        vector<int> nn_idx = query_knn(pts, q, N);
        auto t1 = clock_type::now();
        float approx_search_time = chrono::duration_cast<ms>(t1-t0).count();

        // Compute distances for the approx list (in the same order)
        vector<float> approx_dists; approx_dists.reserve(nn_idx.size());
        for (int id : nn_idx) {
            approx_dists.push_back(euclidean_distance(pts[id], q));
        }

        
        // ============= Search with brute force and the time needed ===========
        t0 = clock_type::now();
        // collect the distances for all the queries and get the N top
        vector<pair<float,int>> all;
        all = brute_force_search(pts, q, N);

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
            // in_range_idx = range_search(pts, q, R); 
            in_range_idx = range_search_fullscan(pts, q, R);
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

        float recall = static_cast<float>(hits_at_N) / N;
        sum_recall += recall;


        // ======================= Approximation Fraction =====================
        float min_approx_dist = *min_element(approx_dists.begin(), approx_dists.end());
        float AF = min_approx_dist / true_topN_dists[0];
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
}


//$./search –d <input file> –q <query file> –k <int> -L <int> -w <float> -ο <output file> -Ν <number of nearest> -R <radius> 
int lsh_main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " -d <input file> -q <query file> -k <int> -L <int> -w <float>" 
                                     << " -o <output file> -N <number of nearest> -R <radius> " << endl;
        return 1;
    }
    Args args = parse_args(argc, argv);

    string type = args.type;
    search_in_dataset(args, type);
    return 0;
}
