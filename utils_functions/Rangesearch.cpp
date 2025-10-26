#include "Rangesearch.h"


float l2sq_cutoff(const vector<float>& a,
                  const vector<float>& b,
                  float cutoff) {
  float acc = 0.f;
  const size_t d = a.size();
  // (Optional) Assume dimensions match; skip checks for speed.
  for (size_t i = 0; i < d; ++i) {
    float diff = a[i] - b[i];
    acc += diff * diff;
    if (acc > cutoff) return acc; // early-abandon
  }
  return acc;
}

vector<int> range_search_fullscan(const vector<vector<float>>& points,
                                  const vector<float>& q,
                                  float R) {
  const float R2 = R * R;
  vector<int> hits;
  hits.reserve(128);

  const int n = static_cast<int>(points.size());
  for (int i = 0; i < n; ++i) {
    float d2 = l2sq_cutoff(points[i], q, R2);
    if (d2 <= R2) hits.push_back(i);
  }
  return hits;
}

vector<pair<int,float>> range_search_fullscan_with_d2(const vector<vector<float>>& points,
                                                      const vector<float>& q,
                                                      float R,
                                                      bool sort_by_dist) {
  const float R2 = R * R;
  vector<pair<int,float>> res;
  res.reserve(128);

  const int n = static_cast<int>(points.size());
  for (int i = 0; i < n; ++i) {
    float d2 = l2sq_cutoff(points[i], q, R2);
    if (d2 <= R2) res.emplace_back(i, d2);
  }

  if (sort_by_dist) {
    sort(res.begin(), res.end(),
         [](const pair<int,float>& a, const pair<int,float>& b) {
           if (a.second != b.second) return a.second < b.second;
           return a.first < b.first;
         });
  }
  return res;
}


