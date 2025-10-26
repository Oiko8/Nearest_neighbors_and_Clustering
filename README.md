# Project 1: Nearest neighbors and Clustering
---
### Team Members:
---
1. -
   -
2. - Name : ***Ioannis Oikonomidis***
   - Email : ***sdi1600120@di.uoa.gr***
 

---

## **Introduction**
In this project we implement several algorithms for Approximate Nearest Neighbors (ANN) search and range search in a n-dimensional space using the Euclidean distance (L2).
The goal is to efficiently find the k nearest vectors to a given vector query, avoiding the computational cost of search with brute force.  

We implemented the following algorithms: 
- ***Locality Sensitive Hashing (LSH)*** : based on random projections that map nearby points to the same hash bucket with high probability.  
It reduces the search space by using multiple hash tables constructed from Gaussian random vectors.
- ***Hypercube*** : an extension of LSH that projects points into a binary hypercube.  
Each vertex of the hypercube corresponds to a binary code derived from the projections, and the search explores neighboring vertices in order of increasing Hamming distance.
- ***IVF-Flat (Inverted File Flat)*** :
- ***IVF-PQ (Inverted File Product Quantization)*** :  

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


### 4. IVF-PQ (Inverted File Product Quantization)

---
## **Program Structure**
### LSH (Euclidean Hashing)

- **Euclidean_Hashing.h (.cpp)** : Builds L hash tables using an amplified hash function g formed by concatenating k base Euclidean (L2) hashes. At query time, it gathers candidates from matching buckets, deduplicates, and re-ranks by L2 to return the nearest neighbors.
- **LSHmain.h (.cpp)** : Performs approximate-NN search for a number of queries and evaluates the performance of the LSH search comparing to a simple exhaustive search. Writes the results in a specified output file. Called via the [search.cpp](src/search.cpp).  

### Hypercube

- **Hypercube.h (.cpp)** : Builds a hash table using as keys the vertices of a binary hypercube. Projects points with Gaussian L2 hashes (from the same function family H as LSH), maps each to a k-bit vertex via a BitMapper, stores buckets by vertex, and at query time visits the base vertex plus up to probes neighbors (ranked by bucket size), taking up to M candidates per visited bucket before exact re-ranking.
- **HCmain.h (.cpp)** : Performs approximate-nn search for a number of queries and evaluates the performance of the Hypercube search comparing to a simple exhaustive search. Prints the results in a specified output file. It is called through the [search.cpp](src/search.cpp).

### Brute Force (Exhaustive search)

- **BruteForceImplementation.h (.cpp)** : Computes exact distances from the query to all points, uses nth_element to keep the N smallest, sorts them, and returns the exact top-N (id, distance) pairs for ground truth and metrics.

### Utils Functions

- **Data_loader.h (.cpp)** : Reads MNIST (big-endian header, 28×28 bytes per image) and SIFT (blocks of dimension followed by 128 floats) into vector<vector<float>>. In MNIST, it reads each pixel as char because the MNIST files store intensity of gray as raw bytes (0–255) and store each value as float because the vectors get normalized from [0,255] to [0,1]. In different occasion the MNIST vectors' values would be stored as integers.
- **Rangesearch.h (.cpp)** : Provides a full-scan R-range search using euclidean distance (L2) with an early-abandon cutoff, plus a variant that optionally returns (id, d²) sorted by distance.
- **euclid.h (.cpp)** : Provides two alternative functions that returns the euclidean distance (L2) of two vectors. One pointers-based that returns the distance² and one vector-based that returns the true distance. The two routines were created in the early stages of the code's building and are kept both due to their usage in different search algorithms.

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
      - [Optional] `-norm` : applies normalization to the mnist dataset ([0-255] -> [0.0-1.0]). Attention to the parameters that must be alternated.
   
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
      - [Optional] `-norm` : applies normalization to the mnist dataset ([0-255] -> [0.0-1.0]). Attention to the parameters that must be alternated.

   3. Approximate-NN search using IVFFlat algorithm :  
      `./search –d <input file> –q <query file> –kclusters <int> -nprobe <int> -ο <output file> -Ν <number of nearest> -R <radius> -type <flag> -range <true|false> -ivfflat –seed <int> `

   4. Approximate-NN search using IVFPQ algorithm :  
      `./search –d <input file> –q <query file> –kclusters <int> -nprobe <int> -M <int> -ο <output file> -Ν <number nearest> -R <radius> -type <flag> -nbits <int> -range <true|false> -ivfpq –seed <int> `
 
 