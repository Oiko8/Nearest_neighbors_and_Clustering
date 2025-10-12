#include "kmeans.h"
#include "euclid.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <random>

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
    // init with kmeans++ for better convergence
    std::vector<size_t> centers_idx;
    centers_idx.reserve(k);
    centers_idx.push_back(unif(rng));
    for(size_t c=1;c<k;c++){
        // compute distances to nearest chosen center
        std::vector<double> dist(n);
        double sum=0;
        for(size_t i=0;i<n;i++){
            double mind = eucliddistance(ds.row(i), &centroids[centers_idx[0]*d], d);
            for(size_t j=1;j<centers_idx.size();j++){
                double dd = eucliddistance(ds.row(i), &centroids[centers_idx[j]*d], d);
                if(dd < mind) mind = dd;
            }
            dist[i] = mind; sum += mind;
        }
        std::uniform_real_distribution<double> r(0, sum);
        double target = r(rng);
        double acc=0; size_t pick=0;
        for(size_t i=0;i<n;i++){ acc += dist[i]; if(acc >= target){ pick=i; break; } }
        centers_idx.push_back(pick);
    }
    // fill initial centroids
    for(size_t c=0;c<k;c++){
        memcpy(&centroids[c*d], ds.row(centers_idx[c]), sizeof(float)*d);
    }

    std::vector<uint32_t> assign(n);
    std::vector<float> newc(k*d);
    std::vector<size_t> counts(k);

    for(size_t it=0; it<max_iters; ++it){
        // assign
        for(size_t i=0;i<n;i++){
            assign[i] = argmin_centroid(ds.row(i), centroids, k, d);
        }
        // zero newc
        std::fill(newc.begin(), newc.end(), 0.0f);
        std::fill(counts.begin(), counts.end(), 0);
        // accumulate
        for(size_t i=0;i<n;i++){
            size_t c = assign[i];
            counts[c]++;
            float* dst = &newc[c*d];
            const float* src = ds.row(i);
            for(size_t j=0;j<d;j++) dst[j] += src[j];
        }
        // finalize centroids (handle empty clusters by reinit random)
        bool converged = true;
        for(size_t c=0;c<k;c++){
            if(counts[c]==0){
                // reinit to random point
                size_t ridx = unif(rng);
                memcpy(&centroids[c*d], ds.row(ridx), sizeof(float)*d);
                converged = false;
                continue;
            }
            for(size_t j=0;j<d;j++){
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
