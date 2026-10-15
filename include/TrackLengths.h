#ifndef DUNE_RNTUPLE_TRACK_LENGTHS_H
#define DUNE_RNTUPLE_TRACK_LENGTHS_H

#include "Schema.h"

inline RVec<double> TrackLengths(const RVec<RVec<V3>> &starts,
                                const RVec<RVec<V3>> &ends) {
  RVec<double> lengths;
  for (std::size_t i = 0; i < starts.size(); ++i) {
    for (std::size_t j = 0; j < starts[i].size(); ++j) {
      lengths.push_back((ends[i][j] - starts[i][j]).R());
    }
  }
  return lengths;
}

#endif
