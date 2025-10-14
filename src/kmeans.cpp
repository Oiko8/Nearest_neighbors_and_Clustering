#include "kmeans.h"
#include "euclid.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <random>
#include <cstring>  // για memcpy
#include <cassert>


static size_t argmin_centroid(const float* v, const std::vector<float> &centroids, size_t k, size_t d){
    size_t best = 0;
    float bestd = eucliddistance(v, &centroids[0], d);
    for(size_t c=1;c<k;c++){
        float dcur = eucliddistance(v, &centroids[c*d], d);
        if(dcur < bestd){ bestd = dcur; best = c; }
    }
    return best;
}

std::vector<float> KMeans::train(const Dataset &ds, size_t k, unsigned seed, size_t max_iters, float tol){
    size_t n = ds.n, d = ds.d;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<size_t> unif(0, n-1);

    std::vector<float> centroids(k * d);

    // --- initialize first centroid randomly ---
    std::vector<size_t> centers_idx;
    centers_idx.reserve(k);
    size_t first = unif(rng);
    centers_idx.push_back(first);
    memcpy(&centroids[0*d], ds.row(first), sizeof(float)*d);

    // --- kmeans++ initialization for remaining centroids ---
    for(size_t c=1; c<k; c++){
        // compute distances to nearest chosen centroid
        std::vector<double> dist(n);
        double sum = 0;
        for(size_t i=0; i<n; i++){
            double mind = eucliddistance(ds.row(i), &centroids[0*d], d);
            for(size_t j=1; j<centers_idx.size(); j++){
                double dd = eucliddistance(ds.row(i), &centroids[j*d], d);
                if(dd < mind) mind = dd;
            }
            dist[i] = mind;
            sum += mind;
        }

        // select new centroid with probability proportional to distance squared
        std::uniform_real_distribution<double> r(0, sum);
        double target = r(rng);
        double acc = 0;
        size_t pick = 0;
        for(size_t i=0; i<n; i++){
            acc += dist[i];
            if(acc >= target){ pick = i; break; }
        }
        centers_idx.push_back(pick);
        memcpy(&centroids[c*d], ds.row(pick), sizeof(float)*d);
    }

    // --- KMeans iterations ---
    std::vector<uint32_t> assign(n);
    std::vector<float> newc(k*d);
    std::vector<size_t> counts(k);

    for(size_t it=0; it<max_iters; ++it){
        // assign points to nearest centroid
        for(size_t i=0; i<n; i++){
            assign[i] = argmin_centroid(ds.row(i), centroids, k, d);
        }

        // zero new centroids
        std::fill(newc.begin(), newc.end(), 0.0f);
        std::fill(counts.begin(), counts.end(), 0);

        // accumulate sums
        for(size_t i=0; i<n; i++){
            size_t c = assign[i];
            counts[c]++;
            float* dst = &newc[c*d];
            const float* src = ds.row(i);
            for(size_t j=0; j<d; j++) dst[j] += src[j];
        }

        // update centroids and check convergence
        bool converged = true;
        for(size_t c=0; c<k; c++){
            if(counts[c] == 0){
                // empty cluster -> reinit to random point
                size_t ridx = unif(rng);
                memcpy(&centroids[c*d], ds.row(ridx), sizeof(float)*d);
                converged = false;
                continue;
            }
            for(size_t j=0; j<d; j++){
                float val = newc[c*d + j] / static_cast<float>(counts[c]);
                if(std::fabs(val - centroids[c*d + j]) > tol) converged = false;
                centroids[c*d + j] = val;
            }
        }

        if(converged) break;
    }

    return centroids;
}


void KMeans::assign_all(const Dataset &ds, const std::vector<float> &centroids, std::vector<uint32_t> &assignments){
    size_t n = ds.n, k = centroids.size()/ds.d;
    assignments.resize(n);
    for(size_t i=0;i<n;i++){
        assignments[i] = argmin_centroid(ds.row(i), centroids, k, ds.d);
    }
}
