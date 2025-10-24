
=================================================================================================================

USING THE MNIST DATASET AND THE TEST DATASET AS QUERIES
We use the mnist dataset now as vectors. We extract and transform the data from the downloaded files.
We use a small python program to transform the files into input.dat and query.dat
We use a small function to extract the data the right wait and transform the binary file from Big-Endian to Small-Endian.

Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
*************************************************
Nearest neighbors for image 0:
47345 27895 
*************************************************
47345 


=================================================================================================================

Results for the first 10 queries.
1-NN search, Range search and brute force search.
The data have been normalised.

oiko@OikoLenovo:~/project/project1/lsh$ ./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 8 -w 6.0 -N 1 -R 5
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Nearest neighbor-1: 35426
True Nearest neighbor-1: 57158
DistanceApproximation: 4.55342
DistanceTrue: 4.39429
R-near neighbors:
8064
12762
35426

Approximation Fraction: 1.03621
----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 10380
True Nearest neighbor-1: 10380
DistanceApproximation: 4.14876
DistanceTrue: 4.14876
R-near neighbors:
10380
21380
29490
33669
37656
40586
46410
51375
56626

Approximation Fraction: 1
----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 28974
True Nearest neighbor-1: 28974
DistanceApproximation: 5.40737
DistanceTrue: 5.40737
R-near neighbors:

Approximation Fraction: 1
----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 51114
True Nearest neighbor-1: 51114
DistanceApproximation: 4.9996
DistanceTrue: 4.9996
R-near neighbors:
51114

Approximation Fraction: 1
----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 34790
True Nearest neighbor-1: 34790
DistanceApproximation: 4.65078
DistanceTrue: 4.65078
R-near neighbors:
34790

Approximation Fraction: 1
----------------------------------------------------------------------------
----------------------------------------------------------------------------
Average AF: 1.00674



=================================================================================================================


RESULTS USING SIFT DATASET WITHOUT NORMALISATION.

oiko@OikoLenovo:~/project/project1/lsh$ ./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 10 -w 35.0 -N 1 -
R 200 -type sift
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Nearest neighbor-1: 565104
True Nearest neighbor-1: 932085
DistanceApproximation: 298.501
DistanceTrue: 232.871
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 842621
True Nearest neighbor-1: 413247
DistanceApproximation: 298.952
DistanceTrue: 226.245
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 249226
True Nearest neighbor-1: 669835
DistanceApproximation: 233.127
DistanceTrue: 175.476
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 190692
True Nearest neighbor-1: 970797
DistanceApproximation: 192.073
DistanceTrue: 174.671
R-near neighbors:
190692

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 417346
True Nearest neighbor-1: 340871
DistanceApproximation: 276.581
DistanceTrue: 214.276
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 657866
True Nearest neighbor-1: 187470
DistanceApproximation: 326.818
DistanceTrue: 253.886
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 834653
True Nearest neighbor-1: 402219
DistanceApproximation: 267.051
DistanceTrue: 224.328
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 262980
True Nearest neighbor-1: 906750
DistanceApproximation: 192.883
DistanceTrue: 154.874
R-near neighbors:
262980

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 475051
True Nearest neighbor-1: 376328
DistanceApproximation: 279.78
DistanceTrue: 213.661
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Nearest neighbor-1: 508376
True Nearest neighbor-1: 178811
DistanceApproximation: 259.569
DistanceTrue: 210.159
R-near neighbors:

----------------------------------------------------------------------------
----------------------------------------------------------------------------
Average AF: 1.25898

=====================================================================================================

EXPERIMENTS WITH ALL THE METRICS IN MNIST DATASET

In the first 10 queries:

./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 12 -w 6.0 -N 5 -R 5 -type mnist

Average AF: 1
Recall@N: 1
QPS: 103.562
tApproximateAverage: 9.65603
tTrueAverage: 31.7636

./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 8 -w 6.0 -N 5 -R 5 -type mnist

Average AF: 1.00841
Recall@N: 0.8
QPS: 154.564
tApproximateAverage: 6.46982
tTrueAverage: 32.8935


In the first 100 queries:

./search -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -k 3 -L 12 -w 6.0 -N 5 -R 5 -type mnist -range false

Average AF: 1.01095
Recall@N: 0.79
QPS: 114.929
tApproximateAverage: 8.70105
tTrueAverage: 32.3744

=====================================================================================================

EXPERIMENTS WITH ALL THE METRICS IN SIFT DATASET

In the first 10 queries:

./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 15 -w 300.0 -N 5 -R 200 -type sift

Average AF: 1.00115
Recall@N: 0.8
QPS: 16.0182
tApproximateAverage: 62.4289
tTrueAverage: 85.2788


./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 8 -w 250.0 -N 5 -R 200 -type sift

Average AF: 1.00274
Recall@N: 0.6
QPS: 38.7276
tApproximateAverage: 25.8214
tTrueAverage: 107.171


In the first 100 queries:

./search -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat  -k 3 -L 8 -w 250.0 -N 5 -R 200 -type sift -range false

Average AF: 1.03148
Recall@N: 0.63
QPS: 40.7431
tApproximateAverage: 24.544
tTrueAverage: 86.1523

==================================================================================================================================

TESTING IN HYPERCUBE



oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w 4.0 -M 50 -probes 4 -N 5 -R 200 -type mnist
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 60000
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784

Number of Vertices with points: 7976
===== Summary over 100 queries =====
Avg approx query time: 9.37 ms
Avg true query time: 22.38 ms
Recall@1 (approx contains any true-1): 95.00 %
Recall@N (average percentage of top-N fully contained): 54.60 %
=================================================
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w
4.0 -M 50 -probes 5 -N 10 -R 200 -type mnist
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 60000
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784

Number of Vertices with points: 7976
===== Summary over 100 queries =====
Avg approx query time: 18.74 ms
Avg true query time: 25.04 ms
Recall@1 (approx contains any true-1): 100.00 %
Recall@N (average percentage of top-N fully contained): 72.10 %
=================================================
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w 4.0 -M 10 -probes 5 -N 10 -R 200 -type mnist
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 60000
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784

Number of Vertices with points: 7976
===== Summary over 100 queries =====
Avg approx query time: 10.12 ms
Avg true query time: 23.48 ms
Recall@1 (approx contains any true-1): 98.00 %
Recall@N (average percentage of top-N fully contained): 48.50 %
=================================================
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w 4.0 -M 100 -probes 4 -N 10 -R 200 -type mnist
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 60000
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784

Number of Vertices with points: 7976
===== Summary over 100 queries =====
Avg approx query time: 27.73 ms
Avg true query time: 24.21 ms
Recall@1 (approx contains any true-1): 99.00 %
Recall@N (average percentage of top-N fully contained): 57.80 %
=================================================
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w 3.0 -M 50 -probes 4 -N 10 -R 200 -type mnist
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 60000
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784

Number of Vertices with points: 10360
===== Summary over 100 queries =====
Avg approx query time: 8.18 ms
Avg true query time: 22.93 ms
Recall@1 (approx contains any true-1): 94.00 %
Recall@N (average percentage of top-N fully contained): 41.90 %
=================================================


### Results in MNIST for hypercube




oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 10 -w 10.0 -M 20 -probes 12 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.05937
Recall@N: 0.4375
QPS: 277.462
tApproximateAverage: 3.6041
tTrueAverage: 25.2811



./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 8 -w 2.0 -M 80 -probes 12 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.14474
Recall@N: 0.155
QPS: 192.02
tApproximateAverage: 5.2078
tTrueAverage: 25.0589



oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 14 -w 20.0 -M 2
0 -probes 4 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.05514
Recall@N: 0.4675
QPS: 250.637
tApproximateAverage: 3.98984
tTrueAverage: 30.7117


oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 12 -w 20.0 -M 2
0 -probes 4 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.04405
Recall@N: 0.5125
QPS: 190.71
tApproximateAverage: 5.24356
tTrueAverage: 30.1532


oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 10 -w 40.0 -M 20 -probes 6 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.0105
Recall@N: 0.85
QPS: 52.8778
tApproximateAverage: 18.9115
tTrueAverage: 29.9541


oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 10 -w 40.0 -M 20 -probes 4 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.01285
Recall@N: 0.84
QPS: 52.8189
tApproximateAverage: 18.9326
tTrueAverage: 31.4667


oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../MNIST_data/input.dat -q ../MNIST_data/query.dat -kproj 10 -w 20.0 -M 20 -probes 4 -N 4 -R 5.0 -type mnist -range false
Loading dataset: ../MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: ../MNIST_data/query.dat
Loaded 10000 test images of dimension 784
Average AF: 1.02963
Recall@N: 0.6075
QPS: 126.228
tApproximateAverage: 7.92219
tTrueAverage: 28.6389

### Results in SIFT for hypercube


oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 250.0 -M 80
 -probes 4 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.22475
Recall@N: 0.18
QPS: 101.535
tApproximateAverage: 9.84885
tTrueAverage: 67.7668
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 12 -w 350.0 -M 80
 -probes 4 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.19139
Recall@N: 0.2075
QPS: 147.662
tApproximateAverage: 6.77222
tTrueAverage: 67.1571
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 450.0 -M 80
 -probes 5 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.06957
Recall@N: 0.575
QPS: 25.5231
tApproximateAverage: 39.1802
tTrueAverage: 69.7709
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 450.0 -M 80 -probes 4 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.07521
Recall@N: 0.5375
QPS: 27.8983
tApproximateAverage: 35.8445
tTrueAverage: 68.8395
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 450.0 -M 80 -probes 2 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.10322
Recall@N: 0.4
QPS: 39.8253
tApproximateAverage: 25.1097
tTrueAverage: 73.4498
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 400.0 -M 80
 -probes 2 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.16114
Recall@N: 0.32
QPS: 62.1454
tApproximateAverage: 16.0913
tTrueAverage: 67.1103
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 10 -w 350.0 -M 80
 -probes 2 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.17332
Recall@N: 0.23
QPS: 94.6716
tApproximateAverage: 10.5628
tTrueAverage: 69.6162
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 14 -w 350.0 -M 80
 -probes 4 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.2685
Recall@N: 0.1375
QPS: 344.024
tApproximateAverage: 2.90677
tTrueAverage: 69.6682
oiko@OikoLenovo:~/project/project1/hypercube$ ./main -d ../SIFT_data/input.dat -q ../SIFT_data/query.dat -kproj 12 -w 350.0 -M 80
 -probes 4 -N 4 -R 5.0 -type sift -range false
Loading dataset: ../SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: ../SIFT_data/query.dat
Loaded 10000 test images of dimension 128
Average AF: 1.19139
Recall@N: 0.2075
QPS: 160.112
tApproximateAverage: 6.24564
tTrueAverage: 68.1722



## Time needed in all the dataset
oiko@OikoLenovo:~/project/project1$ time ./search -d MNIST_data/input.dat -q MNIST_data/query.dat -k 3 -L 12 -w 6.0 -N 5
 -R 2.0 -type mnist -range true -lsh -o results_lsh.txt
Loading dataset: MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: MNIST_data/query.dat
Loaded 10000 test images of dimension 784

real    7m13.890s
user    7m12.268s
sys     0m0.169s
oiko@OikoLenovo:~/project/project1$ time ./search -d MNIST_data/input.dat -q MNIST_data/query.dat -kproj 14 -w 20.0 -M 20 -prob
es 4 -N 4 -R 2.0 -type mnist -range true -hypercube -o results_hc.txt
Loading dataset: MNIST_data/input.dat
Loaded 60000 images of dimension 784
*************************************************
Loading queries: MNIST_data/query.dat
Loaded 10000 test images of dimension 784

real    6m48.180s
user    6m46.679s
sys     0m0.161s

oiko@OikoLenovo:~/project/project1$ time ./search -d SIFT_data/input.dat -q SIFT_data/query.dat  -k 3 -L 8 -w 250.0 -N 5 -R 200
 -type sift -range true -lsh -o results_lsh_sift
Loading dataset: SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: SIFT_data/query.dat
Loaded 10000 test images of dimension 128

real    23m54.952s
user    23m53.865s
sys     0m0.879s
oiko@OikoLenovo:~/project/project1$ time ./search -d SIFT_data/input.dat -q SIFT_data/query.dat -kproj 12 -w 350.0 -M 80 -probes 4 -N 4 -R 5.0 -type sift -range false -hypercube -o results_hc_sift.txt
Loading dataset: SIFT_data/input.dat
Loaded 1000000 images of dimension 128
*************************************************
Loading queries: SIFT_data/query.dat
Loaded 10000 test images of dimension 128

real    13m19.274s
user    13m19.270s
sys     0m0.736s