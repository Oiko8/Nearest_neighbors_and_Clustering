#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// Core distance (squared L2) with early-abandon using cutoff = R^2
float l2sq_cutoff(const vector<float>& a,
                  const vector<float>& b,
                  float cutoff);

// Returns only IDs whose distance <= R
vector<int> range_search_fullscan(const vector<vector<float>>& points,
                                  const vector<float>& q,
                                  float R);

// Returns (id, d2) pairs (optionally sorted by distance asc)
vector<pair<int,float>> range_search_fullscan_with_d2(const vector<vector<float>>& points,
                                                      const vector<float>& q,
                                                      float R,
                                                      bool sort_by_dist = true);
