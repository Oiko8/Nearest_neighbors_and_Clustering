# Approximate - NN Search Report

In the following report, we present the performance and the results of each algorithm in the search of approximate nearest neighbors in a dataset.  
The two dataset that were used are the MNIST and the SIFT dataset with their query-files respectively. We test 4 different algorithms and for each one we experiment on its parameters in order to achieve a good balance between the speed and the results comparing them with an exhaustive search that finds the exact NN but uses too much computational resources and time.

--- 

## LSH Results

### MNIST Dataset

| # | k | L | w | Average AF | Recall@N | QPS | tApprox (ms) | tTrue (ms) |
|---|---|---|---|-------------|-----------|------|---------------|-------------|
| 1 | 3 | 14 | 7.0  | 1.00498 | **0.870** | 100.24  | 9.98  | 22.19 |
| 2 | 4 | 14 | 7.0  | 1.00998 | 0.668 | 297.04  | 3.37  | 22.76 |
| 3 | 4 | 10 | 12.0 | 1.00465 | **0.914** | 74.41   | 13.44 | 23.54 |
| 4 | 5 | 10 | 8.0  | 1.02988 | 0.528 | 744.50  | 1.34  | 21.93 |
| 5 | 5 | 10 | 10.0 | 1.01214 | 0.674 | 306.28  | 3.27  | 23.13 |
| 6 | 6 | 14 | 7.0  | 1.09685 | 0.320 | **1904.62** | **0.53** | 25.33 |
| 7 | 6 | 14 | 8.0  | 1.05645 | 0.434 | 1230.81 | 0.81  | 22.58 |
| 8 | 6 | 14 | 10.0 | 1.02307 | 0.612 | 476.21  | 2.10  | 24.31 |
| 9 | 6 | 12 | 10.0 | 1.03319 | 0.556 | 532.17  | 1.88  | 24.72 |
|10 | 6 | 10 | 12.0 | 1.01269 | 0.722 | 314.68  | 3.18  | 24.27 |

**Observations**
- **Best recall:** `k=4, L=10, w=12.0` → Recall@N = 0.914  
- **Fastest setup:** `k=6, L=14, w=7.0` → QPS = 1904.62  
- **Balanced performance:** `k=3, L=14, w=7.0` → Recall@N = 0.87, AF ≈ 1.00, QPS ≈ 100  

**Trends**
- Increasing **k** reduces recall but increases speed.  
- Increasing **w** improves recall up to a limit, then slows down search.  
- Higher **L** improves recall slightly at the cost of runtime.


### Raw MNIST Dataset

| # | k | L  | w     | Average AF | Recall@N | QPS     | tApprox (ms) | tTrue (ms) |
|---|---|----|-------|------------|----------|---------|--------------|------------|
| 1 | 2 | 14 | 1500  | 1.00309    | **0.938** | 37.907  | 26.3800      | 24.6660    |
| 2 | 3 | 14 | 2000  | **1.00011** | 0.908    | 75.082  | 13.3188      | 22.3167    |
| 3 | 3 | 14 | 1500  | 1.01237    | 0.750    | 152.141 | 6.5728       | 22.3316    |
| 4 | 3 | 10 | 1500  | 1.02460    | 0.638    | 192.316 | 5.1998       | 24.1452    |
| 5 | 4 | 16 | 1500  | 1.03100    | 0.550    | 439.531 | 2.2752       | 23.4568    |
| 6 | 4 | 18 | 1500  | 1.02917    | 0.574    | 398.839 | 2.5073       | 23.0998    |
| 7 | 3 | 14 | 1200  | 1.03131    | 0.546    | 258.630 | 3.8665       | 22.7467    |
| 8 | 3 | 14 | 1000  | 1.05229    | 0.452    | 388.150 | 2.5763       | 23.4190    |
| 9 | 4 | 10 | 1000  | 1.16917    | 0.170    | 2126.27 | 0.4703       | 25.6979    |
|10 | 4 | 10 | 800   | 1.21207    | 0.106    | **4361.98** | **0.2293**     | 24.9744    |

**Observations**
- Because the data are **not normalized** (pixel values ≈ 0–255), pairwise distances are much larger; you need a **much bigger `w`** to get meaningful bucket collisions.  
- **High-accuracy setups:**  
  - `k=2, L=14, w=1500` (Row 1) and `k=3, L=14, w=2000` (Row 2) achieve **Recall@N ≈ 0.94 / 0.91** with AF ≈ 1.00, at the cost of lower QPS.
- **Balanced options:**  
  - `k=3, L=14, w=1500` (Row 3) gives **Recall@N = 0.75** with ~6.6 ms/query (QPS ≈ 152).  
  - `k=3, L=10, w=1500` (Row 4) trades some recall (0.638) for higher throughput.
- **Throughput-focused (low recall):**  
  - Smaller `w` (800–1000) or larger `k` drives **QPS way up** (Rows 9–10) but **recall collapses**.
- **Parameter effects (here):**  
  - ↑`w` → ↑recall (until ~1500–2000), ↓QPS.  
  - ↓`k` → ↑recall, ↓QPS.  
  - ↑`L` → modest ↑recall with roughly linear time cost.



### SIFT Dataset

| # | k | L | w    | Average AF | Recall@N | QPS     | tApprox (ms) | tTrue (ms) |
|---|---|---|------|------------|-----------|---------|--------------|------------|
| 1 | 6 | 16 | 450  | 1.00691    | **0.706** | 88.391  | 11.3133      | 57.5615    |
| 2 | 6 | 14 | 400  | 1.02685    | 0.558     | 142.820 | 7.0018       | 66.4222    |
| 3 | 5 | 14 | 350  | 1.01953    | 0.562     | 114.824 | 8.7090       | 59.8075    |
| 4 | 8 | 14 | 500  | 1.02530    | 0.512     | 254.149 | 3.9347       | 63.1512    |
| 5 | 8 | 12 | 500  | 1.02750    | 0.482     | 314.239 | 3.1823       | 58.9130    |
| 6 | 6 | 10 | 400  | 1.04142    | 0.454     | 198.559 | 5.0363       | 65.7947    |
| 7 | 6 | 16 | 350  | 1.04829    | 0.432     | 234.686 | 4.2610       | 65.4682    |
| 8 | 8 | 10 | 400  | 1.11675    | 0.236     | 1162.25 | 0.8604       | 59.7168    |
| 9 | 8 | 14 | 350  | 1.11819    | 0.214     | 1815.27 | 0.5509       | 60.9708    |
|10 | 8 | 14 | 300  | 1.21424    | 0.118     | 4358.75 | 0.2294       | 58.1795    |

**Observations**
- **Best balanced setup:** `k=6, L=16, w=450.0` → **Recall@N = 0.706**, AF ≈ 1.01, QPS ≈ 88.4.  
  This configuration offers strong accuracy with acceptable query speed — a good default for SIFT.
- **Fast and decent recall:** `k=6, L=14, w=400.0` (Row 2) achieves similar recall (0.56) but faster queries (7 ms).  
- **High throughput but low accuracy:** `k=8, L=14, w=300–350` (Rows 9–10) gives QPS > 1800, but recall ≤ 0.21.  
- **High recall, slower:** `k=4, L=14, w=400.0` (not listed here) reached 0.86 recall but took ~40 ms/query — too slow for large-scale runs.

**Trends**
- Increasing **w** (350 → 450) improves recall substantially, with mild time cost.  
- Larger **L** consistently helps recall (Rows 2 → 1) since more tables improve collision coverage.  
- Increasing **k** reduces recall but accelerates QPS sharply (Rows 1 → 8).  
- SIFT’s high dimensionality (128 D) amplifies sensitivity to **k** and **w**, making tuning crucial.

**Recommendation**
- Report `k=6, L=16, w=450.0` as the **main SIFT–LSH configuration** for balanced performance.  
- Keep `k=8, L=14, w=300–350` as a **speed-focused** comparison and `k=4, L=14, w=400` as a **high-accuracy baseline**.
