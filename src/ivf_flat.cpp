#include "ivf_flat.h"
#include "kmeans.h"
#include "euclid.h"
#include <iostream>
#include <queue>
#include <functional>
#include <algorithm>
#include <cassert>

void IVFFlat::train_and_index(const Dataset &ds, size_t kclusters_, unsigned seed){
    kclusters = kclusters_;
    d = ds.d;
    centroids = KMeans::train(ds, kclusters, seed);
    assert(centroids.size() == kclusters*d);
    lists.assign(kclusters, {});
    // assign points
    std::vector<uint32_t> assign(ds.n);
    KMeans::assign_all(ds, centroids, assign);
    // ME DEBUG //
    for(size_t i=0;i<ds.n;i++){
    if(assign[i] < kclusters){
        std::cerr << "Error: assignment out of bounds! i=" << i << " assign[i]=" << assign[i] << " kclusters=" << kclusters << "\n";
    }
    assert(assign[i] < kclusters);
    lists[ assign[i] ].push_back((uint32_t)i);
}
}

// return top-N pairs (id, dist)
std::vector<std::pair<uint32_t, float>> IVFFlat::search(const float* q, const Dataset &ds, size_t N, size_t nprobe, float R) const {
    // find nprobe nearest centroids (linear scan)
    struct C { size_t idx; float dist; };
    std::vector<C> cent_d(kclusters);
    for(size_t c=0;c<kclusters;c++){
        cent_d[c] = {c, eucliddistance(q, &centroids[c*d], d)};
    }
    std::nth_element(cent_d.begin(), cent_d.begin() + std::min(nprobe, cent_d.size()), cent_d.end(), [](const C&a,const C&b){return a.dist<b.dist;});
    std::vector<size_t> probe_idxs;
    for(size_t i=0;i<std::min(nprobe,cent_d.size());i++) probe_idxs.push_back(cent_d[i].idx);

    // max-heap for top-N (dist, id)
    using PI = std::pair<float, uint32_t>;
    std::priority_queue<PI> heap;

    // ME DEBUG //
    for(size_t p : probe_idxs){
    assert(p < lists.size());
    for(uint32_t id : lists[p]){
        if(id >= ds.n){
            std::cerr << "Error: id out of bounds! id=" << id << " ds.n=" << ds.n << "\n";
        }
        assert(id < ds.n);
            float dist = eucliddistance(q, ds.row(id), d);
            if(R>0 && dist > R*R) continue;
            if(heap.size() < N) heap.emplace(dist, id);
            else if(dist < heap.top().first){
                heap.pop(); heap.emplace(dist, id);
            }
        }
    }
    std::vector<std::pair<uint32_t,float>> result;
    while(!heap.empty()){
        result.emplace_back(heap.top().second, heap.top().first);
        heap.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
