# Approximate - NN Search Report

In the following report, we present the performance and the results of each algorithm in the search of approximate nearest neighbors in a dataset.  
The two dataset that were used are the MNIST and the SIFT dataset with their query-files respectively. We test 4 different algorithms and for each one we experiment on its parameters in order to achieve a good balance between the speed and the results comparing them with an exhaustive search that finds the exact NN but uses too much computational resources and time.  
 
The metrics that we used for the evaluation of the algorithms are described below:
- Average AF : for each query after finding the approximate nearest neighbor and the true nearest neighbor, we calculate the fraction of their distances:
` distance of approximate NN / distance of true NN ` and after we calculate the average.   
With this metric we can estimate how far our search fell off from the true NN. The target is this metric to be as close to `1.0` as possible.
- Recall : after complete the search of k-NN for a query we calculate how many of the approximate NN are in fact in the k true NN and we find the average for each query. This metric defines the accuracy of the model and shows if the hashing or the clustering works as we intended to.  
- QPS : how many queries per second were able to be processed.
- tApprox : average searching time for each query using one of the algorithms we implemented.
- tTrue : average searching time for each query using brute force search.

--- 

## LSH Results

### MNIST Dataset

| # | k | L  | w    | Average AF  | Recall@N  | QPS         | tApprox (ms)  | tTrue (ms)  |
|---|---|----|------|-------------|-----------|-------------|---------------|-------------|
| 1 | 3 | 14 | 7.0  | 1.00498     | **0.870** | 100.24      | 9.98          | 22.19       |
| 2 | 4 | 14 | 7.0  | 1.00998     | 0.668     | 297.04      | 3.37          | 22.76       |
| 3 | 4 | 10 | 12.0 | 1.00465     | **0.914** | 74.41       | 13.44         | 23.54       |
| 4 | 5 | 10 | 8.0  | 1.02988     | 0.528     | 744.50      | 1.34          | 21.93       |
| 5 | 5 | 10 | 10.0 | 1.01214     | 0.674     | 306.28      | 3.27          | 23.13       |
| 6 | 6 | 14 | 7.0  | 1.09685     | 0.320     | **1904.62** | **0.53**      | 25.33       |
| 7 | 6 | 14 | 8.0  | 1.05645     | 0.434     | 1230.81     | 0.81          | 22.58       |
| 8 | 6 | 14 | 10.0 | 1.02307     | 0.612     | 476.21      | 2.10          | 24.31       |
| 9 | 6 | 12 | 10.0 | 1.03319     | 0.556     | 532.17      | 1.88          | 24.72       |
|10 | 6 | 10 | 12.0 | 1.01269     | 0.722     | 314.68      | 3.18          | 24.27       |

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
| #  | k  | L  | w   | Average AF | Recall@N  | QPS        | tApprox (ms) | tTrue (ms) |
| -- | -- | -- | --- | ---------- | --------- | ---------- | ------------ | ---------- |
| 1  | 6  | 20 | 2.2 | 1.00231    | **0.848** | 43.93      | 22.7624      | 81.06      |
| 2  | 6  | 16 | 2.2 | 1.00449    | **0.780** | 56.41      | 17.7275      | 77.34      |
| 3  | 7  | 16 | 2.4 | 1.01390    | **0.734** | 77.91      | 12.8358      | 78.50      |
| 4  | 6  | 14 | 2.0 | 1.01819    | 0.626     | 99.52      | 10.0484      | 73.72      |
| 5  | 8  | 20 | 2.2 | 1.01899    | 0.588     | 179.58     | 5.56848      | 80.42      |
| 6  | 8  | 18 | 2.2 | 1.02261    | 0.562     | 217.80     | 4.59130      | 72.94      |
| 7  | 8  | 16 | 2.2 | 1.02770    | 0.510     | 239.71     | 4.17176      | 77.70      |
| 8  | 10 | 14 | 2.4 | 1.05212    | 0.380     | **586.79** | **1.70418**  | 77.91      |
| 9  | 8  | 14 | 2.0 | 1.06264    | 0.372     | 445.55     | 2.24443      | 76.79      |
| 10 | 6  | 10 | 1.5 | 1.06837    | 0.294     | 468.06     | 2.13648      | 73.09      |


**Observations**  
- Because the SIFT data are **normalized**, the distances between points are smaller compared to the raw dataset. As a result, smaller **w** values (around 2.0–2.4) are required to achieve a good balance between speed and recall.  
- **Best recall:** `k=6, L=20, w=2.2` → Recall@N = 0.848. This configuration provides the highest accuracy among the tests, achieving very high recall and a near-perfect Average AF. The average search time remains under 25 ms, making it the best option when accuracy is the main objective.   
- **High-accurate setup:** `k=7, L=16, w=2.4` → Recall@N = 0.734. Slightly higher `w` helps stabilize recall while keeping the time low (≈12.8 ms), giving a solid compromise between precision and performance.  
- **Fastest setup:** `k=10, L=14, w=2.4` → QPS = 586.79. This configuration achieves extremely fast searches (≈1.7 ms per query) with a lower recall (0.380), clearly demonstrating the trade-off between speed and accuracy at higher `k` values.   
- **Balanced performance 1:** `k=6, L=14, w=2.0` → Recall@N = 0.626, AF = 1.018, QPS ≈ 100. This setup achieves a strong balance between recall and query time, providing reliable results without excessive cost.  
- **Balanced performance 2:** `k=8, L=18, w=2.2` → Recall@N = 0.562, AF = 1.023, QPS ≈ 218. It performs consistently with good recall and smooth timing, showing the effectiveness of slightly increasing both `L` and `w`.  
- **Balanced performance 3:** `k=8, L=20, w=2.2` → Recall@N = 0.588, AF = 1.019, QPS ≈ 180. This configuration offers moderate recall and fast response, proving that increasing `L` further improves stability without large time penalties.  
These results confirm the same general behavior as with other datasets — smaller **k**, larger **L**, and slightly higher **w** values usually improve recall but increase computation time. The final configuration depends on whether the focus is on achieving higher precision or maximizing search speed.




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
| 5 | 10    | 12  | 20 | 10     | 1.06108    | 0.5100     | 238.279     | 4.19676      | 26.5158    |
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
| #  | kproj | w   | M  | probes | Average AF | Recall@N   | QPS         | tApprox (ms) | tTrue (ms) |
| -- | ----- | --- | -- | ------ | ---------- | ---------- | ----------- | ------------ | ---------- |
| 1  | 16    | 4.0 | 20 | 10     | 1.02303    | **0.6225** | 33.6213     | 29.7430      | 76.8701    |
| 2  | 18    | 4.0 | 20 | 14     | 1.04606    | **0.5550** | 51.1595     | 19.5467      | 78.8525    |
| 3  | 22    | 4.0 | 20 | 20     | 1.05642    | **0.5100** | 80.4090     | 12.4364      | 75.5884    |
| 4  | 14    | 3.0 | 20 | 10     | 1.09165    | 0.4850     | 42.0761     | 23.7664      | 77.0154    |
| 5  | 22    | 4.0 | 20 | 10     | 1.06775    | 0.4925     | 78.7843     | 12.6929      | 80.0880    |
| 6  | 24    | 4.0 | 20 | 20     | 1.06718    | 0.4725     | 122.577     | 8.15814      | 78.7375    |
| 7  | 24    | 5.0 | 20 | 10     | 1.06816    | 0.4875     | 75.5720     | 13.2324      | 79.5392    |
| 8  | 24    | 3.0 | 20 | 10     | 1.25572    | 0.2100     | **459.233** | **2.17754**  | 78.1771    |
| 9  | 22    | 3.0 | 20 | 10     | 1.23935    | 0.2275     | 285.296     | 3.50513      | 74.7092    |
| 10 | 22    | 3.0 | 20 | 16     | 1.22306    | 0.2375     | 263.585     | 3.79384      | 79.0142    |


**Observations**  
- Because the SIFT data are **normalized**, the distances are tighter than in raw space. In the hypercube, slightly **larger `w` (≈3–4)** with a reasonable `kproj` and a few **probes** tends to lift recall while keeping time controlled.  
- **Best recall:** `kproj=10, w=3.5, M=20, probes=8` → Recall@N = **0.8075**, AF ≈ **1.023**. This configuration gives the highest accuracy among the tests. The average search time is longer (~74.9 ms), but it achieves precise results, making it a strong choice when accuracy is the main objective.  
- **Fastest setup:** `kproj=24, w=3.0, M=20, probes=10` → **tApprox ≈ 2.18 ms**, QPS ≈ **459.2**, Recall@N = **0.210**. It clearly demonstrates the speed–accuracy trade-off: aggressive projection with low probes is extremely efficient but reduces recall.  
- **Balanced performance 1:** `kproj=22, w=4.0, M=20, probes=20` → Recall@N = **0.510**, AF ≈ **1.056**, QPS ≈ **80.4**, tApprox ≈ **12.44 ms**. Solid balance with recall above 0.5 and smooth latency.  
- **Balanced performance 2:** `kproj=24, w=4.0, M=20, probes=14` → Recall@N = **0.4725**, AF ≈ **1.067**, QPS ≈ **122.6**, tApprox ≈ **8.158 ms**. Slightly less accurate than the balance setup 1 but with faster time and still a low AF. Ideal for a fast balanced performance.  
These results show the same general behavior as elsewhere — increasing **probes** (and sometimes `w`) improves recall, while higher **kproj** with fewer probes gives much faster queries at the cost of precision. The final choice depends on whether you favor **throughput** or **accuracy** for the task at hand.

#### **Trends on HYPERCUBE search**

- Increasing **kproj** (more hash bits) makes buckets **sparser** → **higher QPS / lower tApprox**, but **lower recall** unless compensated by larger `w`, more `probes`, or higher `M`.
- Increasing **probes** (extra vertices visited) **improves recall** and slightly lowers AF, with **near-linear cost** in query time; a clean knob to trade a bit of speed for accuracy.
- Increasing **M** (max points examined) raises recall up to a plateau; beyond that, **diminishing returns** while `tApprox` keeps growing.
- Increasing **w** (L2 window) raises collision probability for close points → **better recall** and lower AF, but can enlarge buckets and **slow queries**. Too small `w` under-collides and hurts recall.
- **Parameter interactions:** larger **kproj** typically needs either **more probes** or **larger w** to sustain recall; smaller **kproj** with modest `w` keeps recall high but increases per-query time.
- **Normalization matters:** normalized datasets require **smaller `w`** (e.g., SIFT ≈ 2–3.5, MNIST ≈ 10–40 in your runs); tune `w` to the distance scale.
- **Balanced profiles** in your results: moderate **kproj**, medium **w**, **probes ≈ 6–8**, **M ≈ 20–40** → **good recall** with `tApprox` often **< 20 ms**.
- For **maximum speed**, push **kproj↑** and **probes↓/M↓**; for **maximum recall**, push **probes↑/M↑** and **w↑** (accept higher `tApprox`).


<br><br>

--- 
---

<br><br>


## IVFFLAT Results

### MNIST Dataset

| # | Clusters (C) | Probes | Average AF | Recall@N  | QPS         | tApprox (ms) | tTrue (ms) |
| - | ------------ | ------ | ---------- | --------- | ----------- | ------------ | ---------- |
| 1 | 64           | 2      | 1.01016    | 0.920     | **1282.05** | **0.78**     | 24.13      |
| 2 | 64           | 4      | 1.00103    | **0.990** | 540.541     | 1.85         | 23.60      |
| 3 | 128          | 2      | 1.00821    | 0.870     | **8333.33** | **0.12**     | 24.03      |
| 4 | 128          | 4      | 1.00051    | **0.980** | 1111.11     | 0.90         | 23.38      |


**Observations**  
- **Best recall:** `C=64, probes=4` → Recall@N = **0.99**, AF = **1.001**, QPS ≈ 540. This setup achieves almost perfect recall with minimal loss in speed, showing that slightly increasing the number of probes drastically boosts accuracy.  
- **Fastest setup:** `C=128, probes=2` → QPS = **8333.33** (≈0.12 ms/query), Recall@N = 0.87. Extremely fast but with a small trade-off in recall, highlighting IVFFlat’s scalability when few clusters are explored.  
- **Balanced performance 1:** `C=64, probes=2` → Recall@N = **0.92**, AF = 1.010, QPS = 1282. This configuration balances speed and precision effectively.  
- **Balanced performance 2:** `C=128, probes=4` → Recall@N = **0.98**, AF = **1.0005**, QPS = 1111. Very high accuracy and still under 1 ms average query time.  

Overall, the **IVFFlat** method performs impressively on MNIST — recall reaches nearly exact search levels (≈ 0.99) with extremely low AF (≈ 1.00) and sub-millisecond average query times. The algorithm benefits clearly from increasing **probes**, while the number of **clusters** mainly affects indexing granularity and search speed.


### SIFT Dataset

| # | Clusters (C) | Probes | Average AF | Recall@N  | QPS         | tApprox (ms) | tTrue (ms) |
| - | ------------ | ------ | ---------- | --------- | ----------- | ------------ | ---------- |
| 1 | 64           | 2      | 0.65005    | 0.640     | **171.821** | **5.82**     | 60.74      |
| 2 | 64           | 4      | 0.68000    | **0.680** | 83.963      | 11.91        | 61.81      |
| 3 | 128          | 2      | 0.68005    | 0.670     | 159.744     | 6.26         | 64.88      |
| 4 | 128          | 4      | 0.67900    | 0.660     | 83.963      | 11.91        | 61.81      |


**Observations**  
- **Best recall:** `C=64, probes=4` → Recall@N = **0.68**, AF = 0.68. This configuration provides the highest recall among the tested setups, indicating that increasing the number of probes slightly improves retrieval quality.  
- **Fastest setup:** `C=64, probes=2` → QPS = **171.82**, tApprox ≈ **5.82 ms**, showing the clear trade-off between speed and recall. Even with fewer probes, it maintains reasonable accuracy (Recall = 0.64).  
- **Balanced performance 1:** `C=128, probes=2` → Recall@N = **0.67**, AF = 0.68, QPS ≈ 160. This setting achieves a good midpoint between quality and speed.  
- **Balanced performance 2:** `C=128, probes=4` → Recall@N = **0.66**, AF = 0.679, slightly slower but consistent in stability, with higher clustering resolution.  