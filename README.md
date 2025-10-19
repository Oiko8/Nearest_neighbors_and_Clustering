# Project 1: Nearest neighbors and Clustering
---
### Team Members:
---
1. -
   -
2. - Name : ***Ioannis Oikonomidis***
   - Email : ***sdi1600120@di.uoa.gr***
 

---

## Introduction
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

## Implementation

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
## Program Structure