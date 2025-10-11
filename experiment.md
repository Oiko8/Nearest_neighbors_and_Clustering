[LSH vs Brute force in the search of nearest neighbor]

The difference is super big when we use a hash table and search only in the bucket of the similar points
than searching all the points one by one. Set of 10000 points is used.
**********************************************************
Search for nearest neighor for a point
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.17941,2.3966,0.461232,)
Distance: 0.338216
Brute-force total: 0.250092
**********************************************************
Search for nearest neighor for a point through the hash table
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.17941,2.3966,0.461232,)
Distance: 0.338216
LSH (L=1) total: 0.000732
**********************************************************


=================================================================================================================

USE OF L HASH TABLES EACH ONE CONNECTED TO ONE AMPLIFIED HASH FUNCTION G(p)

We search in L different hash tables to ensure that the real nearest neighbor will be found
As we see in the most cases the nearest neighbor of the query is in the same bucket with the query. 
But at the end we need this only in one hash table, and we ensure that we find the correct nearest neighbor
Even searching L=8 tables is faster than comparing all the points one by one.
Set of 100000 points is used.

**********************************************************
Search for nearest neighor for a point
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
Brute-force total: 1.73295
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.48184,2.20735,0.200077,)
Distance: 0.317715
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.48184,2.20735,0.200077,)
Distance: 0.317715
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.13278,2.32067,0.412281,)
Distance: 0.256558
**********************************************************
**********************************************************
Point to examine: (5.2,2.1,0.3,)
Closest point: (5.48184,2.20735,0.200077,)
Distance: 0.317715
**********************************************************
LSH (L=8) total: 0.088316


=================================================================================================================

USE L=4 HASH TABLES IN A SET OF 100000 4-D POINTS 
As we see it is getting more difficult to ensure that the bucket of the query contains the nearest neighbor.
But as long as we use multiple hash tables, we have a lot of probabilities to find the closest point.

**********************************************************
Search for nearest neighor for a point
Point to examine: (6.20197,1.25505,11.1429,13.1611,)
Closest point: (6.75236,1.55604,11.5259,13.0235,)
Distance: 0.74774
Brute-force total: 1.48682
**********************************************************
**********************************************************
Point to examine: (6.20197,1.25505,11.1429,13.1611,)
Closest point: (6.75236,1.55604,11.5259,13.0235,)
Distance: 0.74774
**********************************************************
**********************************************************
Point to examine: (6.20197,1.25505,11.1429,13.1611,)
Closest point: (5.93789,1.2823,10.8353,14.1076,)
Distance: 1.03009
**********************************************************
**********************************************************
Point to examine: (6.20197,1.25505,11.1429,13.1611,)
Closest point: (6.75236,1.55604,11.5259,13.0235,)
Distance: 0.74774
**********************************************************
**********************************************************
Point to examine: (6.20197,1.25505,11.1429,13.1611,)
Closest point: (6.73281,1.20422,12.3809,13.4335,)
Distance: 1.37522
**********************************************************
LSH (L=4) total: 0.116854

=================================================================================================================

ADDING THE RANGE SEARCH 

**********************************************************
Points in range R=1.2 from the query point:
(5.93789, 1.2823, 10.8353, 14.1076, )
(6.75236, 1.55604, 11.5259, 13.0235, )
(5.81402, 0.924696, 10.9248, 12.4796, )
**********************************************************

=================================================================================================================

TRANSFORM THE NN QUERY SO IT RETURNS K NEAREST NEIGHBOR.
We combine the NN query with the range search. Set of 1000000 points.

**********************************************************
K=5 Nearest Neighbor in each bucket:
(6.21835, 1.19587, 10.8274, 12.9356, )
(6.38571, 1.35712, 10.7692, 13.0479, )
(6.00677, 0.818914, 11.3963, 13.0485, )
(6.39896, 0.978496, 11.569, 13.0249, )
(6.19614, 1.75079, 11.4658, 13.086, )
**********************************************************
Points in range R=0.6 from the query point:
(6.39896, 0.978496, 11.569, 13.0249, )
(6.21835, 1.19587, 10.8274, 12.9356, )
(6.38571, 1.35712, 10.7692, 13.0479, )
(6.19614, 1.75079, 11.4658, 13.086, )
(6.00677, 0.818914, 11.3963, 13.0485, )
**********************************************************


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