#!/bin/bash

# Αρχείο εκτέλεσης
EXEC=./search

# Δεδομένα
MNIST_INPUT=MNIST_data/input.dat
MNIST_QUERY=MNIST_data/query.dat
SIFT_INPUT=SIFT_data/input.dat
SIFT_QUERY=SIFT_data/query.dat

# Δημιουργία λίστας σετ παραμέτρων (kclusters nprobe M nbits seed)
# IVF-FLAT: μόνο kclusters, nprobe, seed
IVF_FLAT_PARAMS=(
    "64 2 1"
    "128 4 42"
    "256 8 123"
    "512 16 7"
    "1024 16 2025"
    "256 2 0"
    "128 8 99"
    "64 4 11"
    "512 8 77"
    "1024 32 5"
)

# IVFPQ: kclusters nprobe M nbits seed
IVF_PQ_PARAMS=(
    "64 2 8 8 1"
    "128 4 8 8 42"
    "256 8 16 8 123"
    "512 16 16 8 7"
    "1024 16 32 8 2025"
    "256 2 8 4 0"
    "128 8 8 4 99"
    "64 4 4 8 11"
    "512 8 16 4 77"
    "1024 32 32 8 5"
)

echo "=== Running IVF-FLAT on MNIST ==="
for params in "${IVF_FLAT_PARAMS[@]}"; do
    read k nprobe seed <<< $params
    echo "Running kclusters=$k nprobe=$nprobe seed=$seed"
    $EXEC -ivfflat -d $MNIST_INPUT -q $MNIST_QUERY -type mnist -kclusters $k -nprobe $nprobe -N 1 -R 10000 -range true -seed $seed
done

echo "=== Running IVF-FLAT on SIFT ==="
for params in "${IVF_FLAT_PARAMS[@]}"; do
    read k nprobe seed <<< $params
    echo "Running kclusters=$k nprobe=$nprobe seed=$seed"
    $EXEC -ivfflat -d $SIFT_INPUT -q $SIFT_QUERY -type sift -kclusters $k -nprobe $nprobe -N 1 -R 200 -range true -seed $seed
done

echo "=== Running IVFPQ on MNIST ==="
for params in "${IVF_PQ_PARAMS[@]}"; do
    read k nprobe M nbits seed <<< $params
    echo "Running kclusters=$k nprobe=$nprobe M=$M nbits=$nbits seed=$seed"
    $EXEC -ivfpq -d $MNIST_INPUT -q $MNIST_QUERY -type mnist -kclusters $k -nprobe $nprobe -M $M -nbits $nbits -N 1 -R 10000 -range true -seed $seed
done

echo "=== Running IVFPQ on SIFT ==="
for params in "${IVF_PQ_PARAMS[@]}"; do
    read k nprobe M nbits seed <<< $params
    echo "Running kclusters=$k nprobe=$nprobe M=$M nbits=$nbits seed=$seed"
    $EXEC -ivfpq -d $SIFT_INPUT -q $SIFT_QUERY -type sift -kclusters $k -nprobe $nprobe -M $M -nbits $nbits -N 1 -R 200 -range true -seed $seed
done

echo "=== All tests finished ==="
