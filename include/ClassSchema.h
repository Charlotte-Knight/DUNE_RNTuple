#ifndef DUNE_RNTUPLE_CLASS_SCHEMA_H
#define DUNE_RNTUPLE_CLASS_SCHEMA_H

#include <cmath>
#include <cstdint>
#include <vector>

namespace classschema {

class V3 {
 public:
  V3() = default;
  V3(double xIn, double yIn, double zIn) : x(xIn), y(yIn), z(zIn) {}

  double X() const { return x; }
  double Y() const { return y; }
  double Z() const { return z; }

  double Mag2() const { return x * x + y * y + z * z; }
  double Mag() const { return std::sqrt(Mag2()); }

  V3 Unit() const {
    const double mag = Mag();
    return mag > 0.0 ? (*this) * (1.0 / mag) : V3();
  }

  V3 operator+(const V3 &other) const {
    return V3(x + other.x, y + other.y, z + other.z);
  }

  V3 operator-(const V3 &other) const {
    return V3(x - other.x, y - other.y, z - other.z);
  }

  V3 operator*(double scale) const { return V3(x * scale, y * scale, z * scale); }

  friend V3 operator*(double scale, const V3 &value) { return value * scale; }

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

enum class PartEMethod : std::uint8_t {
  kUnknownMethod = 0,
  kRange = 1,
  kMCS = 2,
  kCalorimetry = 3,
  kVisibleEnergy = 4
};

enum class RecoObjType : std::uint8_t {
  kUnknownRecoObj = 0,
  kTrack = 1,
  kShower = 2,
  kHitCollection = 3,
};

class Particle {
 public:
  double MomentumMagnitude() const { return p.Mag(); }
  double TrackLength() const { return (end - start).Mag(); }

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
  std::vector<std::uint64_t> children;
  std::vector<std::uint64_t> truth;
  std::vector<float> truthOverlap;
};

class Interaction {
 public:
  std::uint64_t id = 0;
  float Enu = 0.f;
  V3 vtx;
  std::vector<Particle> part;
  std::vector<std::uint64_t> truth;
  std::vector<float> truthOverlap;
};

class TruthParticle {
 public:
  double MomentumMagnitude() const { return p.Mag(); }
  double TrackLength() const { return (end - start).Mag(); }

  std::uint64_t id = 0;
  bool primary = false;
  int pdg = 0;
  int tgtA = 0;
  float E = 0.f;
  V3 p;
  V3 start;
  V3 end;
  std::uint64_t parent = 0;
  std::vector<std::uint64_t> children;
};

class TruthInteraction {
 public:
  std::uint64_t id = 0;
  float Enu = 0.f;
  V3 vtx;
  std::vector<TruthParticle> part;
  std::vector<std::uint64_t> reco;
  std::vector<float> recoOverlap;
};

class Common {
 public:
  std::uint64_t id = 0;
  std::vector<Interaction> dlp;
  std::vector<Interaction> pandora;
};

class Mc {
 public:
  std::uint64_t id = 0;
  std::vector<TruthInteraction> interactions;
};

}  // namespace classschema

#endif
