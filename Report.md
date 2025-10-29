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
| #  | k | L  | w   | Average AF | Recall@N  | QPS         | tApprox (ms) | tTrue (ms) |
| -- | - | -- | --- | ---------- | --------- | ----------- | ------------ | ---------- |
| 1  | 3 | 10 | 1.0 | 1.0137     | **0.664** | 30.16       | 33.16        | 82.62      |
| 2  | 4 | 10 | 1.0 | 1.0348     | 0.444     | 117.68      | 8.50         | 85.27      |
| 3  | 5 | 12 | 1.0 | 1.0725     | 0.290     | 371.01      | 2.70         | 76.11      |
| 4  | 5 | 12 | 1.5 | 1.0292     | 0.608     | 76.46       | 13.08        | 78.65      |
| 5  | 5 | 14 | 1.5 | 1.0152     | 0.652     | 68.53       | 14.59        | 77.31      |
| 6  | 6 | 14 | 1.0 | 1.1413     | 0.174     | **1003.95** | **1.00**     | 79.59      |
| 7  | 6 | 16 | 1.5 | 1.0383     | 0.532     | 136.71      | 7.31         | 88.57      |
| 8  | 8 | 12 | 1.8 | 1.0414     | 0.382     | 388.03      | 2.58         | 77.79      |
| 9  | 8 | 12 | 2.0 | 1.0239     | 0.506     | 202.01      | 4.95         | 87.85      |
| 10 | 8 | 10 | 2.0 | 1.0380     | 0.440     | 291.77      | 3.43         | 77.57      |

**Observations**  
- Because the SIFT data are **normalized**, the distances between points are smaller compared to the raw dataset. As a result, smaller **`w`** values (around 1–2) are required to achieve a good balance between speed and recall.  
- **Best recall:** `k=3, L=10, w=1.0` → Recall@N = 0.664. This configuration provides the highest accuracy among the tests, with relatively high recall and a low Average AF. The average search time is longer, but it achieves precise results, making it a strong choice when accuracy is the main objective.  
- **Fastest setup:** `k=6, L=14, w=1.0` → QPS = 1003.95. This setup gives extremely fast searches (1.00 ms per query) with lower recall (0.174). It clearly demonstrates the trade-off between speed and accuracy, as a higher `k` drastically reduces recall but provides excellent efficiency.  
- **Balanced performance 1:** `k=6, L=16, w=1.5` → Recall@N = 0.532, AF = 1.038, QPS ≈ 137. This configuration offers a strong balance between recall and query time, delivering reliable accuracy while being considerably faster than brute-force search.  
- **Balanced performance 2:** `k=8, L=12, w=2.0` → Recall@N = 0.506, AF = 1.024, QPS ≈ 202. This setup performs faster while maintaining moderate recall, showing that slightly increasing `w` helps recover accuracy without major performance loss.  
These results confirm the same behavior as with other datasets — small **k**, larger **L**, and slightly higher **w** values generally yield better recall but slower performance. The final choice depends on whether speed or precision is prioritized in the search task.



### Raw SIFT Dataset

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

   
<br><br>

--- 
---

<br><br>

## HYPERCUBE Results

### MNIST Dataset

| # | kproj | w   | M  | probes | Average AF | Recall@N   | QPS         | tApprox (ms) | tTrue (ms) |
|---|-------|-----|----|--------|------------|------------|-------------|--------------|------------|
| 1 | 18    | 20  | 20 | 6      | 1.08418    | 0.3800     | **558.152** | **1.79163**  | 28.5365    |
| 2 | 18    | 18  | 20 | 10     | 1.07775    | 0.4050     | 543.876     | 1.83865      | 27.0732    |
| 3 | 18    | 30  | 20 | 4      | 1.06201    | 0.4575     | 321.332     | 3.11205      | 29.6644    |
| 4 | 18    | 20  | 20 | 20     | 1.05718    | 0.4800     | 345.404     | 2.89516      | 28.4821    |
| 5 | 10    | 12  | 50 | 10     | 1.06108    | 0.5100     | 238.279     | 4.19676      | 26.5158    |
| 6 | 8     | 8   | 20 | 10     | 1.03737    | 0.5275     | 183.453     | 5.45098      | 28.5480    |
| 7 | 8     | 10  | 50 | 8      | 1.03852    | 0.5400     | 199.936     | 5.00160      | 27.7670    |
| 8 | 18    | 40  | 20 | 6      | 1.03214    | 0.6350     | 168.409     | 5.93791      | 28.9569    |
| 9 | 10    | 20  | 20 | 6      | 1.02168    | 0.6675     | 115.978     | 8.62235      | 26.6772    |
|10 | 14    | 20  | 20 | 20     | 1.02664    | **0.7075** | 105.035     | 9.52061      | 29.6014    |

**Observations**  
- **Best recall:** `kproj=14, w=20, M=20, probes=20` → Recall@N = **0.7075**, AF = 1.02664, QPS ≈ 105. This configuration gives the highest accuracy among all runs, maintaining a very low AF and still reasonable query speed.  
- **Fastest setup:** `kproj=18, w=20, M=20, probes=6` → QPS = **558.152** (≈1.79 ms/query), Recall@N = 0.38. This shows the typical trade-off — very high speed but lower recall, mainly due to the aggressive projection and fewer probes.  
- **Balanced performance 1:** `kproj=18, w=40, M=20, probes=6` → Recall@N = **0.635**, AF = 1.032, QPS ≈ 168. A strong overall setup where both accuracy and response time stay competitive.  
- **Balanced performance 2:** `kproj=8, w=10, M=50, probes=8` → Recall@N = **0.540**, AF = 1.038, QPS ≈ 200. This gives a good balance between speed and quality, making it a reliable mid-range choice for the MNIST dataset.




### SIFT Dataset
| # | kproj | w   | M  | probes | Average AF | Recall@N   | QPS     | tApprox (ms) | tTrue (ms) |
|---|-------|-----|----|--------|------------|------------|---------|--------------|------------|
| 1 | 12    | 2.0 | 20 | 6      | 1.06052    | **0.5150** | 36.856  | 27.133       | 72.2342    |
| 2 | 16    | 3.0 | 10 | 7      | 1.05615    | 0.5100     | 56.725  | 17.629       | 65.1428    |
| 3 | 16    | 3.0 | 40 | 6      | 1.05923    | 0.4975     | 57.847  | 17.287       | 67.3098    |
| 4 | 22    | 3.5 | 40 | 8      | 1.09800    | 0.4650     | 81.943  | 12.204       | 67.6209    |
| 5 | 18    | 3.0 | 40 | 8      | 1.06783    | 0.4450     | 82.817  | 12.075       | 70.0159    |
| 6 | 22    | 3.3 | 40 | 8      | 1.09407    | 0.4250     | 104.584 | 9.562        | 69.3955    |
| 7 | 8     | 1.0 | 10 | 8      | 1.08687    | 0.4175     | 30.366  | 32.932       | 71.5136    |
| 8 | 20    | 3.0 | 40 | 10     | 1.09143    | 0.4025     | 107.207 | **9.328**    | 78.9875    |
| 9 | 16    | 2.5 | 10 | 7      | 1.12195    | 0.3500     | 102.736 | 9.734        | 69.4440    |
|10 | 14    | 2.0 | 10 | 6      | 1.12317    | 0.3475     | 106.739 | 9.369        | 65.0338    |

**Observations**  
- **Best recall:** `kproj=12, w=2.0, M=20, probes=6` → Recall@N = **0.515**, AF = 1.061, QPS ≈ 36.9, tApprox ≈ 27.1 ms. Strongest accuracy within the filtered set.  
- **Fastest setup (in this top-10):** `kproj=20, w=3.0, M=40, probes=10` → **tApprox ≈ 9.33 ms**, QPS ≈ 107.2, Recall@N = 0.4025. Best latency while keeping recall > 0.4.  
- **Balanced performance 1:** `kproj=16, w=3.0, M=40, probes=6` → Recall@N = **0.4975**, AF = 1.059, QPS ≈ 57.8, tApprox ≈ 17.3 ms.  
- **Balanced performance 2:** `kproj=22, w=3.3, M=40, probes=8` → Recall@N = **0.425**, AF = 1.094, QPS ≈ 104.6, tApprox ≈ 9.56 ms.

#### **Trends on HYPERCUBE search**

- Increasing **kproj** (more hash bits) makes buckets **sparser** → **higher QPS / lower tApprox**, but **lower recall** unless compensated by larger `w`, more `probes`, or higher `M`.
- Increasing **probes** (extra vertices visited) **improves recall** and slightly lowers AF, with **near-linear cost** in query time; a clean knob to trade a bit of speed for accuracy.
- Increasing **M** (max points examined) raises recall up to a plateau; beyond that, **diminishing returns** while `tApprox` keeps growing.
- Increasing **w** (L2 window) raises collision probability for close points → **better recall** and lower AF, but can enlarge buckets and **slow queries**. Too small `w` under-collides and hurts recall.
- **Parameter interactions:** larger **kproj** typically needs either **more probes** or **larger w** to sustain recall; smaller **kproj** with modest `w` keeps recall high but increases per-query time.
- **Normalization matters:** normalized datasets require **smaller `w`** (e.g., SIFT ≈ 2–3.5, MNIST ≈ 10–40 in your runs); tune `w` to the distance scale.
- **Balanced profiles** in your results: moderate **kproj**, medium **w**, **probes ≈ 6–8**, **M ≈ 20–40** → **good recall** with `tApprox` often **< 20 ms**.
- For **maximum speed**, push **kproj↑** and **probes↓/M↓**; for **maximum recall**, push **probes↑/M↑** and **w↑** (accept higher `tApprox`).




