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
- **Best recall:** `k=4, L=10, w=12.0` → Recall@N = 0.914. The metrics are really good, but the average time of the approximate search is too long, something that we try to avoid.   
- **Fastest setup:** `k=6, L=14, w=7.0` → QPS = 1904.62. This setup provides a really fast approximate search, however the recall is low (→ Recall@N = 0.32). The average AF is not too big, a fact that shows that the distance of approximate nearest neigbor for each query is generally close to the distance of the true nearest neighbor. If speed is what we are looking for, this setup is really fast with acceptable results. 
- **Balanced performance 1:** `k=6, L=10, w=12.0` → Recall@N = 0.72, AF = 1.013, QPS ≈ 314.   
- **Balanced performance 2:** `k=6, L=14, w=10.0` → Recall@N = 0.61, AF = 1.023, QPS ≈ 476.  
The last two setups provides a really effective performance with high recalls and fast results. They are approximately 10 times faster than the exhaustive search and with pretty precise results.  

**Trends**
- Increasing **k** reduces recall but increases speed.  
- Increasing **w** improves recall up to a limit, then slows down search.  
- Higher **L** improves recall slightly at the cost of runtime.


### Raw MNIST Dataset

| # | k | L  | w     | Average AF | Recall@N | QPS     | tApprox (ms) | tTrue (ms) |
|---|---|----|-------|------------|----------|---------|--------------|------------|
| 1 | 3 | 14 | 2000  | **1.00011** | **0.908** | 75.082  | 13.3188      | 22.3167    |
| 2 | 4 | 14 | 2500  | 1.00215    | 0.880    | 100.984 | 9.9026       | 23.8404    |
| 3 | 6 | 14 | 3000  | 1.00483    | 0.806    | 237.969 | 4.2022       | 25.1847    |
| 4 | 6 | 14 | 2000  | 1.06741    | 0.410    | **1202.21** | **0.8318** | 25.4071    |
| 5 | 4 | 14 | 2000  | 1.00356    | 0.758    | 193.533 | 5.1671       | 25.0713    |
| 6 | 3 | 14 | 1500  | 1.01237    | 0.750    | 152.141 | 6.5728       | 22.3316    |
| 7 | 6 | 10 | 3000  | 1.01497    | 0.696    | 309.946 | 3.2264       | 26.3634    |
| 8 | 4 | 12 | 2000  | 1.00949    | 0.698    | 208.418 | 4.7981       | 27.4284    |
| 9 | 4 | 10 | 2000  | 1.01656    | 0.656    | 269.518 | 3.7103       | 24.5089    |
|10 | 6 | 14 | 2500  | 1.02672    | 0.584    | 486.637 | 2.0549       | 24.7020    |

**Observations**
- Because the data are **not normalized** (pixel values ≈ 0–255), pairwise distances are much larger; we need a **much bigger `w`** to get a hig rate of collisions in each bucket.  
- **Best recall:** `k=3, L=14, w=2000.0` → Recall@N = 0.908. The metrics are really good, but the average time of the approximate search is too long, something that we try to avoid.   
- **Fastest setup:** `k=6, L=14, w=2000.0` → QPS = 1202. This setup gives us really fast performance with 0.83 ms per approximate search on average. The recall is relatively acceptable (Recall@N = 0.41) with a low Average AF, too. For an effective search focused on speed this setup is ideal. 
- **Balanced performance 1:** `k=6, L=10, w=3000.0` → Recall@N = 0.696, AF = 1.015, QPS ≈ 309.   
- **Balanced performance 2:** `k=6, L=14, w=3000.0` → Recall@N = 0.806, AF = 1.005, QPS ≈ 238.  
These 2 sets of parameters provide the more balanced performance with small differences. The first one is almost 9 times faster than the brute force search with a recall close to 0.7. The other is slightly slower but with really high recall equal to 0.806.  
The final choice depends on the dataset and the need for speed and accuracy.

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
- **Best recall:** `k=6, L=16, w=450.0` → Recall@N = 0.706. This configuration provides the best accuracy among the tests, with acceptable query time and low Average AF, making it a strong general-purpose setup for the SIFT dataset.  
- **Fastest setup:** `k=8, L=14, w=300.0` → QPS = 4358.75. This setup achieves extremely fast approximate searches (0.23 ms per query) but with very low recall (0.12). It highlights the trade-off between speed and accuracy when `k` is too high.  
- **Balanced performance 1:** `k=6, L=14, w=400.0` → Recall@N = 0.558, AF = 1.027, QPS ≈ 143. This configuration combines good accuracy and high speed, providing results nearly ten times faster than brute-force search.  
- **Balanced performance 2:** `k=5, L=14, w=350.0` → Recall@N = 0.562, AF = 1.019, QPS ≈ 115. Another solid setup with similar accuracy but slightly lower recall and speed compared to the previous one.  


#### **Trends on LSH search**
- Increasing **k** reduces recall but increases speed.  
- Increasing **w** improves recall up to a certain point, and then it slows down the search.  
- Increasing **L** slightly improves recall, but it also increases the average query time almost linearly.  
- The same behavior is observed in all datasets (MNIST, raw MNIST, and SIFT). LSH follows a consistent trade-off between accuracy and speed, and the parameters affect the performance in a similar way.  
- Small values of **k**, large **L**, and wide **w** give higher recall and accuracy. On the other hand, larger **k**, smaller **L**, and narrower **w** make the algorithm much faster but less accurate.