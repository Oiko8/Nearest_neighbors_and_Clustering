# Project 1: Nearest neighbors and Clustering
---
### Team Members:
---
1. - Name : ***Polyxeni Kollia***
   - Email : ***sdi1800280@di.uoa.gr***
   - AM : 1115201800280
2. - Name : ***Ioannis Oikonomidis***
   - Email : ***sdi1600120@di.uoa.gr***
   - AM : 1115201600120
 

---

## **Introduction**
In this project we implement several algorithms for Approximate Nearest Neighbors (ANN) search and range search in a n-dimensional space using the Euclidean distance (L2).
The goal is to efficiently find the k nearest vectors to a given vector query, avoiding the computational cost of search with brute force.  

We implemented the following algorithms: 
- ***Locality Sensitive Hashing (LSH)*** : based on random projections that map nearby points to the same hash bucket with high probability.  
It reduces the search space by using multiple hash tables constructed from Gaussian random vectors.
- ***Hypercube*** : an extension of LSH that projects points into a binary hypercube.  
Each vertex of the hypercube corresponds to a binary code derived from the projections, and the search explores neighboring vertices in order of increasing Hamming distance.
- ***IVF-Flat (Inverted File Flat)*** : partitions the dataset into k clusters (centroids) via k-means. Each data vector is assigned to its nearest centroid. Searching is done by probing the nearest nprobe centroids and computing exact distances only within those lists.
- ***IVF-PQ (Inverted File Product Quantization)*** : extends IVF-Flat by quantizing the subspaces of vectors using Product Quantization (PQ). Each subvector is approximated using a codebook, and approximate distances are computed via precomputed lookup tables (LT) for efficiency.

### Datasets

1. **MNIST** - 60,000 handwritten digit images of 28×28 pixels (784-dimensional integer vectors). 
2. **SIFT** - one million 128-dimensional feature vectors extracted from real images, stored in 32-bit floating-point format.

### Output (Evaluation Metrics)
The output file contains the name of the algorithm that is used in the search along with the results for each query and following metrics for evaluation : 
- Average Approximation Factor (AF)
- Recall@N (percentage of true nearest neighbors found)
- Queries per Second (QPS)
- Average query time for approximate and exact search

---

## **Implementation**

### 1. LSH (Euclidean LSH)
We implement classic Euclidean LSH using Gaussian random projections and amplified hash functions g built from multiple h_i. Each h projects a point p onto a random vector v, adds a random shift t ∈ [0,w), divides by window w and floors to an integer bin:  
` h(p) = ⌊ (v*p + t) / w ⌋ `  
Amplified functions combine k such h (with integer coefficients r_i) into an ID that is then reduced to a table index. The code that builds LSH tables, the Hash and AmplifiedHash classes, and the querying routines are implemented in [Euclidean_Hashing.cpp](lsh/Euclidean_Hashing.cpp) / [Euclidean_Hashing.h](lsh/Euclidean_Hashing.h).  

Key implementation notes:
- We create L amplified hash structures (each with k base h functions) and build an unordered_map per table mapping table index → list of point ids.

- KNN queries collect candidate points from each table’s bucket for the query, keep the best k per table, deduplicate, then select global top-k by exact Euclidean distance.

### 2. Hypercube (randomized projections + binary mapping)  
The Hypercube implementation projects points using multiple Gaussian projection lines (same Hash idea as in LSH) and maps each projection to one bit to form a k-bit vertex string (a hypercube vertex). Points are inserted into an unordered_map<string, vector<int>> keyed by vertex string. The core hypercube implementation (hash generation, bit mapping, table build, and query helpers) is in [Hypercube.cpp](hypercube/Hypercube.cpp) / [Hypercube.h](hypercube/Hypercube.h).  

Key implementation notes:

- build_hypercube(...) creates k independent Hash projections and per-bit BitMapper objects, computes each point’s k-bit vertex, and appends the point id to that vertex bucket.

- For queries, vertex_for_point(q) computes the base vertex for q. We then probe neighboring vertices in Hamming order (or use a bucket-size-guided probe order) and collect up to M closest candidates per visited vertex. Candidate gathering and post-filtering are implemented in gather_candidates(...), then KNN/Range results are computed from that candidate set.

- Two helper strategies for selecting neighbor vertices are provided: simple Hamming-order enumeration and a top-by-bucket strategy that prioritizes adjacent vertices with larger bucket sizes (faster candidate accumulation with fewer probes).

### 3. IVF-Flat (Inverted File Flat)
- Files: ivf_flat.h/.cpp, ivfflat_main.cpp

Key Classes / Functions:

   - IVFFlat::train_and_index(const Dataset &ds, size_t kclusters, unsigned seed)
      - Trains kmeans centroids on the dataset.
      - Assigns each vector to its nearest centroid cluster.
      - Populates lists[centroid_index] with vector IDs.
   Why: Partitioning the dataset into clusters reduces search space for ANN queries.

   - IVFFlat::search(const float* q, const Dataset &ds, size_t N, size_t nprobe, float R)
      - Finds the nprobe nearest centroids to query q.
      - Computes exact L2 distances for vectors in these clusters.
      - Maintains a max-heap of top-N nearest neighbors.
   Why: Efficient ANN search by restricting distance calculations to nearby clusters.

   - ivfflat_main.cpp
      - Command-line interface for training, indexing, and querying with IVF-Flat.
      - Handles loading datasets, parsing parameters, and writing results.

### 4. IVF-PQ (Inverted File Product Quantization)
- Files: ivf_pq.h/.cpp, ivfpq_main.cpp

Key Classes / Functions:
   - PQCode → Stores quantized subvector codes for a single vector.

   - IVFPQ::train(const Dataset &ds, size_t kclusters_, size_t M_, size_t nbits_, unsigned seed)
      - Trains IVF centroids using kmeans.
      - Splits each vector into M subspaces and trains a K-means codebook for each subspace (product quantization).
   Why: Reduces memory and computational cost by encoding vectors with compact PQ codes.

   -IVFPQ::index_dataset(const Dataset &ds)
      - Assigns each vector to its closest IVF cluster.
      - Computes PQ code for each subvector.
      - Stores (id, PQCode) pairs in inverted lists.

   - IVFPQ::search(const float* q, const Dataset &ds, size_t N, size_t nprobe, float R)
      - Computes lookup tables (LT) for the query vs all codebook centroids.
      - Probes nearest clusters and sums LT values to get approximate distances.
      - Maintains top-N max-heap.
   Why: ANN search with PQ provides fast approximate distance computation using precomputed tables.

   - ivfpq_main.cpp
      - Command-line interface for training, indexing, and querying with IVF-PQ.

---
## **Program Structure**
### LSH (Euclidean Hashing)

- **Euclidean_Hashing.h (.cpp)** : Builds L hash tables using an amplified hash function g formed by concatenating k base Euclidean (L2) hashes. At query time, it gathers candidates from matching buckets, deduplicates, and re-ranks by L2 to return the nearest neighbors.
- **LSHmain.h (.cpp)** : Performs approximate-NN search for a number of queries and evaluates the performance of the LSH search comparing to a simple exhaustive search. Writes the results in a specified output file. Called via the [search.cpp](src/search.cpp).  

### Hypercube

- **Hypercube.h (.cpp)** : Builds a hash table using as keys the vertices of a binary hypercube. Projects points with Gaussian L2 hashes (from the same function family H as LSH), maps each to a k-bit vertex via a BitMapper, stores buckets by vertex, and at query time visits the base vertex plus up to probes neighbors (ranked by bucket size), taking up to M candidates per visited bucket before exact re-ranking.
- **HCmain.h (.cpp)** : Performs approximate-nn search for a number of queries and evaluates the performance of the Hypercube search comparing to a simple exhaustive search. Prints the results in a specified output file. It is called through the [search.cpp](src/search.cpp).

### IVFflat
- ivf_flat.h (.cpp): Defines the IVFFlat struct and functions for training (k-means for IVF centroids) and indexing.
   - train_and_index:
     Trains IVF centroids using k-means on the dataset.
     Assigns each vector to its nearest centroid.
     Creates inverted lists per cluster storing the vector IDs.
   - search:
      Finds the nprobe nearest centroids for a query.
      Scans the vectors in those clusters, computing exact distances.
      Returns top-N results (id, distance) with optional radius R filtering.
   - ivflat_main.cpp (.h): Contains the ivfflat_main function that launches the IVF-Flat program:
      Parses command-line parameters (input, query, N, kclusters, nprobe, R, output file, etc.).
      Calls train_and_index for training and indexing.
      Executes search for each query vector and writes results to the output file.

### IVF-PQ
   - ivf_pq.h (.cpp): Defines the IVFPQ struct and functions for training, indexing, and search:
      - train:
        Trains IVF centroids for coarse quantization.
        Splits each vector into M subvectors and trains PQ codebooks per subspace using k-means.
   - index_dataset:
        Assigns each vector to the nearest IVF centroid.
        Encodes each subvector using the corresponding PQ codebook.
        Stores (id, PQCode) pairs in inverted lists per cluster.
   - search:
        Finds nprobe nearest IVF centroids.
        Precomputes lookup tables (LT) for each query subvector and each codebook centroid.
        Scans vectors in selected clusters, computes approximate distances using LT, applies radius filter R, and keeps top-N results.
   - ivfpq_main.cpp (.h): Contains the ivfpq_main function:
        Parses command-line parameters (input, query, N, kclusters, nprobe, M, nbits, R, output file, etc.).
        Calls train for IVF centroids and PQ codebooks training.
        Calls index_dataset to build inverted lists.
        Executes search for each query vector and writes results to the output file.

### Utils Functions

- **Data_loader.h (.cpp)** : Reads MNIST (big-endian header, 28×28 bytes per image) and SIFT (blocks of dimension followed by 128 floats) into vector<vector<float>>. In MNIST, it reads each pixel as char because the MNIST files store intensity of gray as raw bytes (0–255) and store each value as float because the vectors get normalized from [0,255] to [0,1]. In different occasion the MNIST vectors' values would be stored as integers.
- **Rangesearch.h (.cpp)** : Provides a full-scan R-range search using euclidean distance (L2) with an early-abandon cutoff, plus a variant that optionally returns (id, d²) sorted by distance.
- **euclid.h (.cpp)** : Provides two alternative functions that returns the euclidean distance (L2) of two vectors. One pointers-based that returns the distance² and one vector-based that returns the true distance. The two routines were created in the early stages of the code's building and are kept both due to their usage in different search algorithms.
- **nearest_neighbor.h (.cpp)** : Implements a brute-force nearest neighbor search on a dataset.
-  **nearest_neighbor (const Dataset &ds, const float* q, int N, vector<uint32_t> &out_ids, vector<float> &out_dists)** : Computes distances from query q to all vectors in ds. Keeps the top-N nearest vectors using nth_element for efficiency. Sorts the final N vectors in ascending order of distance. Returns the results as out_ids (vector indices) and out_dists (distances).
   - Implementation Notes:
      nearest_neighbor uses squared L2 distance during selection to avoid unnecessary square roots for efficiency. The final distances are converted to true Euclidean distances using sqrt. Reserving memory for         vectors (all, out_ids, out_dists) improves performance when working with large datasets like MNIST or SIFT.

This function is used by algorithms IVFFlat and IVFPQ as the exact search baseline for evaluating approximate nearest neighbor.
---
## **Compilation and Execution**

- Compilation: `make`  
   All algorithms are compiled into the same executable search. You can select which one to run via the corresponding flag (-lsh, -hypercube, -ivfflat, -ivfpq).
- Execution:   
   1. Approximate-NN search using LSH algorithm :  
      `./search –d <input file> –q <query file> –k <int> -L <int> -w <double> -ο <output file> -Ν <number of nearest> -R <radius> -type <flag> -lsh -range <true|false>`
      - `-d <input file>` : contains the set of vectors
      - `–q <query file>` : contains the query vectors
      - `–k <int>` : number of hash functions hi that are used to define g()
      - `-L <int>` : number of hash tables
      - `-w <double>` : window used for hashing on the projection line
      - `-ο <output file>` : contains the results of the search
      - `-Ν <number of nearest>` : number of NN 
      - `-R <radius>`  : radius in which the algorithm searches for points from the query 
      - `-type <flag>` : the dataset that is used (mnist or sift)
      - `-range <true|false>` : activates (true) or deactivates (false) range search
      - `-lsh` : the method (algorithm) that is used in the search
      - [Optional] `-raw` : does not apply normalization to the datasets. Attention to the parameters that must be alternated.  
      - [Optional] `-subset <number of queries>` : the algorithm only runs a subset of queries and not all the query file. 
      
      Example :
      - ` ./search -d MNIST_data/input.dat -q MNIST_data/query.dat -k 4 -L 14 -w 6.0 -N 5 -R 3.0 -type mnist -range true -lsh -o ../results/results_lsh_mnist.txt -subset 100`
      - ` ./search -d SIFT_data/input.dat -q SIFT_data/query.dat -k 6 -L 14 -w 2.0 -N 5 -R 1.0 -type sift -range true -lsh -o ../results/results_lsh_sift.txt -subset 100`
   
   
   2. Approximate-NN search using Hypercube algorithm :  
      `./search –d <input file> –q <query file> –kproj <int> -w <double> -M <int> -probes <int> -ο <output file> -Ν <number of nearest> -R <radius> -type <flag> -range <true|false> -hypercube`
      - `–d <input file>` : contains the set of vectors
      - `–q <query file>` : contains the query vectors
      - `–kproj <int>` : the dimension of the hypercube in which the vectors are projected 
      - `-w <double>` : window used for hashing on the projection line
      - `-M <int>` : max number of points that are checked in each edge of the hypercube
      - `-probes <int>` : max number of edges that are checked for NN
      - `-ο <output file>` : contains the results of the search
      - `-Ν <number of nearest>` : number of NN
      - `-R <radius>` : radius in which the algorithm searches for points from the query
      - `-type <flag>` : the dataset that is used (mnist or sift)
      - `-range <true|false>` : activates (true) or deactivates (false) range search
      - `-hypercube` : the method (algorithm) that is used in the search
      - [Optional] `-raw` : does not apply normalization to the datasets. Attention to the parameters that must be alternated.
      - [Optional] `-subset <number of queries>` : the algorithm only runs a subset of queries and not all the query file.  

      Example :
      - `./search -d MNIST_data/input.dat -q MNIST_data/query.dat -kproj 10 -w 12.0 -M 20 -probes 10 -N 4 -R 2.0 -type mnist -range true -hypercube -o ../results/results_hc_mnist.txt -subset 100`
      - `./search -d SIFT_data/input.dat -q SIFT_data/query.dat -kproj 22 -w 4.0 -M 20 -probes 10 -N 4 -R 1.0 -type sift -range true -hypercube -o ../results/results_hc_sift.txt -subset 100`

   3. Approximate-NN search using IVFFlat algorithm :
      -d <input file> : contains the dataset vectors
      -q <query file> : contains the query vectors
      -kclusters <int> : number of IVF clusters to train
      -nprobe <int> : number of closest clusters to probe during search
      -ο <output file> : file to store the search results
      -Ν <number of nearest> : number of nearest neighbors to return
      -R <radius> : optional radius for range search (squared distance cutoff)
      -type <flag> : dataset type (mnist or sift)
      -range <true|false> : enable or disable range search
      -ivfflat : select IVF-Flat as the search method
      -seed <int> : random seed for k-means training
      Usage :
      `./search –d <input file> –q <query file> –kclusters <int> -nprobe <int> -ο <output file> -Ν <number of nearest> -R <radius> -type <flag> -range <true|false> -ivfflat –seed <int> `
   4. Approximate-NN search using IVFPQ algorithm :
      -d <input file> : dataset vectors
      -q <query file> : query vectors
      -kclusters <int> : number of IVF clusters to train
      -nprobe <int> : number of closest clusters to probe
      -M <int> : number of subspaces for PQ encoding
      -ο <output file> : file to store search results
      -Ν <number nearest> : number of nearest neighbors to return
      -R <radius> : optional radius for range search
      -type <flag> : dataset type (mnist or sift)
      -nbits <int> : number of bits per subvector for PQ (K = 2^nbits)
      -range <true|false> : enable or disable range search
      -ivfpq : select IVF-PQ as the search method
      -seed <int> : random seed for k-means training (both IVF and PQ codebooks)
      Usage :
      `./search –d <input file> –q <query file> –kclusters <int> -nprobe <int> -M <int> -ο <output file> -Ν <number nearest> -R <radius> -type <flag> -nbits <int> -range <true|false> -ivfpq –seed <int> `


 ***NOTES*** : 
 - The metrics for the search and a variety of different combinations of parameters are provided in [Report.md](Report.md).
 - The results of the search can be found in [results folder](results/).
