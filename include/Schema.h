#ifndef DUNE_RNTUPLE_SCHEMA_H
#define DUNE_RNTUPLE_SCHEMA_H

#include <cstdint>
#include <vector>

#include <Math/Vector3D.h>
#include <ROOT/RVec.hxx>

using V3 = ROOT::Math::XYZVector;
template <typename T>
using RVec = ROOT::VecOps::RVec<T>;

enum class PartEMethod : std::uint8_t {
  kUnknownMethod = 0,
  kRange = 1,         ///< Amount of material traversed by particle
  kMCS = 2,           ///< Multiple scattering
  kCalorimetry = 3,   ///< From deposited energy (with possible invisible-energy corrections)
  kVisibleEnergy = 4  ///< From collected photons with calibration
};

enum class RecoObjType : std::uint8_t {
  kUnknownRecoObj = 0, ///< default value
  kTrack          = 1,  ///< track
  kShower         = 2,  ///< shower
  kHitCollection  = 3,  ///< hit collection (mostly used to garbage collect all remaining hits)
};

struct Particle {
  std::uint64_t id = 0;
  bool primary = false;
  int pdg = 0;
  int tgtA = 0;
  float score = 0.f;
  float E = 0.f;
  PartEMethod EMethod = PartEMethod::kUnknownMethod;
  V3 p;
  V3 start;
  V3 end;
  bool contained = false;
  float walldist = 0.f;
  RecoObjType recoType = RecoObjType::kUnknownRecoObj;
  std::uint64_t parent = 0;
  RVec<std::uint64_t> children;
  RVec<std::uint64_t> truth;
  RVec<float> truthOverlap;
};

struct Interaction {
  std::uint64_t id = 0;
  float Enu = 0.f;
  V3 vtx;
  RVec<Particle> part;
  RVec<std::uint64_t> truth;
  RVec<float> truthOverlap;
};

struct TruthParticle {
  std::uint64_t id = 0;
  bool primary = false;
  int pdg = 0;
  int tgtA = 0;
  float E = 0.f;
  V3 p;
  V3 start;
  V3 end;
  std::uint64_t parent = 0;
  RVec<std::uint64_t> children;
};

struct TruthInteraction {
  std::uint64_t id = 0;
  float Enu = 0.f;
  V3 vtx;
  RVec<TruthParticle> part;
  RVec<std::uint64_t> reco;
  RVec<float> recoOverlap;
  // more fields to be added here to catch up with current CAF
};

struct common {
  std::uint64_t id = 0;
  RVec<Interaction> dlp;
  RVec<Interaction> pandora;
};

struct mc {
  std::uint64_t id = 0;
  RVec<TruthInteraction> interactions;
};

#endif
