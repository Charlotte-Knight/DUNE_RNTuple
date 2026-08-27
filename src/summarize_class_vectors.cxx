#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include <ROOT/RNTupleReader.hxx>

#include "ClassSchema.h"

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills_class_schema.root";

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<classschema::Common>("common");
  auto mcView = reader->GetView<classschema::Mc>("mc");

  std::size_t recoParticleCount = 0;
  std::size_t truthParticleCount = 0;
  double recoMomentumMagnitudeSum = 0.0;
  double recoTrackLengthSum = 0.0;
  double truthMomentumMagnitudeSum = 0.0;

  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    const auto mcEntry = mcView(entryId);

    auto accumulateReco = [&](const std::vector<classschema::Interaction> &interactions) {
      for (const auto &interaction : interactions) {
        for (const auto &particle : interaction.part) {
          ++recoParticleCount;
          recoMomentumMagnitudeSum += particle.p.Mag();
          recoTrackLengthSum += particle.TrackLength();
        }
      }
    };

    accumulateReco(commonEntry.dlp);
    accumulateReco(commonEntry.pandora);

    for (const auto &interaction : mcEntry.interactions) {
      for (const auto &particle : interaction.part) {
        ++truthParticleCount;
        truthMomentumMagnitudeSum += particle.MomentumMagnitude();
      }
    }
  }

  std::cout << "Reco particles: " << recoParticleCount << '\n';
  std::cout << "Average reco |p| via V3::Mag(): "
            << (recoParticleCount > 0
                    ? recoMomentumMagnitudeSum / static_cast<double>(recoParticleCount)
                    : 0.0)
            << '\n';
  std::cout << "Average reco track length via Particle::TrackLength(): "
            << (recoParticleCount > 0
                    ? recoTrackLengthSum / static_cast<double>(recoParticleCount)
                    : 0.0)
            << '\n';
  std::cout << "Truth particles: " << truthParticleCount << '\n';
  std::cout << "Average truth |p| via TruthParticle::MomentumMagnitude(): "
            << (truthParticleCount > 0
                    ? truthMomentumMagnitudeSum / static_cast<double>(truthParticleCount)
                    : 0.0)
            << '\n';

  return 0;
}
