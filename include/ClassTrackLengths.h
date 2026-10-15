#ifndef DUNE_RNTUPLE_CLASS_TRACK_LENGTHS_H
#define DUNE_RNTUPLE_CLASS_TRACK_LENGTHS_H

#include <ROOT/RVec.hxx>

#include "ClassSchema.h"

namespace classschema {
inline ROOT::VecOps::RVec<double> TrackLengths(
    const ROOT::VecOps::RVec<ROOT::VecOps::RVec<V3>> &starts,
    const ROOT::VecOps::RVec<ROOT::VecOps::RVec<V3>> &ends) {
  ROOT::VecOps::RVec<double> lengths;
  for (std::size_t i = 0; i < starts.size(); ++i) {
    for (std::size_t j = 0; j < starts[i].size(); ++j) {
      lengths.push_back((ends[i][j] - starts[i][j]).Mag());
    }
  }
  return lengths;
}
}

#endif
