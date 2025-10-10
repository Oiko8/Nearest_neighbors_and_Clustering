// Usage : 
// MNIST : ./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 12 -w 6.0 -N 5 -R 5 -type mnist -range true
// SIFT  : ./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 8 -w 250.0 -N 5 -R 200 -type sift -range true

#include "Euclidean_Hashing.h"
#include "Data_loader.h"
#include <fstream>

using clock_type = std::chrono::steady_clock;
using ms = std::chrono::duration<double, std::milli>;

struct Args {
    string data_path;         // -d
    string query_path;        // -q (optional)
    string type = "mnist";    // -type mnist|sift
    int k = 1;                // -N
    int L = 5;                // -L
    int khash = 4;            // -k
    double w = 4.0;           // -w
    double R = 2000.0;        // -R (MNIST default)
    bool range = false;       // -range (if there will be a range search)
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
        else if (flag=="-range"){ need(1); a.range=str2bool(argv[++i]); }
        
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


void search_in_dataset(Args args , string type){
    vector<vector<double>> pts;
    vector<vector<double>> queries;
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
            for (double &dim : point) {
                dim /= 255.0;
            }
        }
        for (auto &query : queries){
            for (double &dim : query) {
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
    double w = args.w;
    
    // Prepare the tables according to the data
    build_hash_tables(pts, L, khash, w);


    vector<double> q;

    double sum_AF = 0.0;
    int qcount = 0;
    int hits_at_N = 0;                 // Recall@N counter
    double sum_tApprox_ms = 0.0;       // sum of the time for the approx search
    double sum_tTrue_ms   = 0.0;       // sum of the time for the true search 
 
    // check the first 10 queries
    for (int i=0 ; i<100 ; i++){
        q = queries[i];


        // =====================================================================
        // ============================ NN Search===============================
        // =====================================================================

        // ============= Approximate search (LSH) and the time needed ==========
        auto t0 = clock_type::now();
        vector<int> nn_idx = query_knn(pts, q, N);
        auto t1 = clock_type::now();
        double approx_search_time = chrono::duration_cast<ms>(t1-t0).count();

        // Compute distances for the approx list (in the same order)
        vector<double> approx_dists; approx_dists.reserve(nn_idx.size());
        for (int id : nn_idx) {
            approx_dists.push_back(euclidean_distance(pts[id], q));
        }

        
        // ============= Search with brute force and the time needed ===========
        t0 = clock_type::now();
        // collect the distances for all the queries and get the N top
        vector<pair<double,int>> all;
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
        double true_search_time = chrono::duration_cast<ms>(t1 - t0).count();

        vector<int> true_topN_ids;
        true_topN_ids.reserve(all.size());
        vector<double> true_topN_dists;
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
            double R = args.R;
            vector<int> in_range_idx = range_search(pts, q, R);         
        }


        // =====================================================================
        // ========================= Prepare the metrics =======================
        // =====================================================================
        
        // ========================= Recall @N =================================
        // if the true nearest neighbor exists in the N approx nearest neighbors
        int true_nearest_id = true_topN_ids[0];
        for (int id : nn_idx) {
            if (id == true_nearest_id) {
                hits_at_N ++;
                break;
            }
        }

        // ======================= Approximation Fraction =====================
        double min_approx_dist = *min_element(approx_dists.begin(), approx_dists.end());
        double AF = min_approx_dist / true_topN_dists[0];
        sum_AF += AF;
        sum_tApprox_ms += approx_search_time;
        sum_tTrue_ms   += true_search_time;
        qcount++;
        


        // =====================================================================
        // ============================== Results ==============================
        // =====================================================================
        cout << "Query: " << i+1 << "\n";
        for (int i = 0; i < (int)nn_idx.size(); ++i) {
            cout << "Nearest neighbor-" << (i+1) << ": " << nn_idx[i] << "\n";
            cout << "distanceApproximate: " << approx_dists[i] << "\n";
            // If we have fewer true items than i+1 (shouldn’t happen), clamp
            double dtrue_i = true_topN_dists[ min(i, (int)true_topN_dists.size()-1) ];
            cout << "distanceTrue: " << dtrue_i << "\n";
        }
        
        if (args.range == true) {
            cout << "\nR-near neighbors:\n";
            for (int id : in_range_idx) cout << id << "\n";
        }
        cout << "\n";
    }
    // =========================================================================
    // ========================== Summary ======================================
    // =========================================================================

    cout << "Average AF: " << (qcount ? (sum_AF / qcount) : 0.0) << "\n";
    cout << "Recall@N: "   << (qcount ? (double)hits_at_N / qcount : 0.0) << "\n";

    double tApproxAvg = qcount ? (sum_tApprox_ms / qcount) : 0.0;
    double tTrueAvg   = qcount ? (sum_tTrue_ms   / qcount) : 0.0;
    cout << "QPS: " << (sum_tApprox_ms > 0.0 ? (qcount / (sum_tApprox_ms / 1000.0)) : 0.0) << "\n";
    cout << "tApproximateAverage: " << tApproxAvg << "\n";
    cout << "tTrueAverage: "        << tTrueAvg   << "\n";
}


//$./search –d <input file> –q <query file> –k <int> -L <int> -w <double> -ο <output file> -Ν <number of nearest> -R <radius> 
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " -d <input file> -q <query file> -k <int> -L <int> -w <double>" 
                                     << " -o <output file> -N <number of nearest> -R <radius> " << endl;
        return 1;
    }
    Args args = parse_args(argc, argv);

    string type = args.type;
    search_in_dataset(args, type);
    return 0;
}
