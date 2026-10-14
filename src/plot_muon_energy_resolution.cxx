#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include <ROOT/RNTupleReader.hxx>
#include <TCanvas.h>
#include <TH1D.h>
#include <TROOT.h>

#include "Schema.h"

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills.root";
  const std::string outputImage =
      (argc > 2) ? argv[2] : "build/muon_energy_resolution.png";

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<common>("common");
  auto mcView = reader->GetView<mc>("mc");

  TH1D resolutionHistogram("hMuonEnergyResolution",
                           "Muon energy resolution;(E_{reco}-E_{true})/E_{true};Particles",
                           120, -1.5, 1.5);

  std::size_t nFilled = 0;
  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    const auto mcEntry = mcView(entryId);

    std::unordered_map<std::uint64_t, const TruthParticle *> truthMuonById;
    for (const auto &truthInteraction : mcEntry.interactions) {
      for (const auto &truthParticle : truthInteraction.part) {
        if (std::abs(truthParticle.pdg) == 13 && truthParticle.E > 0.f) {
          truthMuonById[truthParticle.id] = &truthParticle;
        }
      }
    }

    auto fillFromRecoInteractions = [&](const RVec<Interaction> &interactions) {
      for (const auto &interaction : interactions) {
        for (const auto &particle : interaction.part) {
          if (std::abs(particle.pdg) != 13 || particle.E <= 0.f) {
            continue;
          }

          const TruthParticle *bestTruthMuon = nullptr;
          float bestOverlap = -1.f;
          for (std::size_t i = 0; i < particle.truth.size(); ++i) {
            const auto truthId = particle.truth[i];
            const auto foundTruth = truthMuonById.find(truthId);
            if (foundTruth == truthMuonById.end()) {
              continue;
            }
            const float overlap =
                (i < particle.truthOverlap.size()) ? particle.truthOverlap[i] : 0.f;
            if (overlap > bestOverlap) {
              bestOverlap = overlap;
              bestTruthMuon = foundTruth->second;
            }
          }

          if (bestTruthMuon == nullptr || bestTruthMuon->E <= 0.f) {
            continue;
          }

          const double resolution = (static_cast<double>(particle.E) -
                                     static_cast<double>(bestTruthMuon->E)) /
                                    static_cast<double>(bestTruthMuon->E);
          resolutionHistogram.Fill(resolution);
          ++nFilled;
        }
      }
    };

    fillFromRecoInteractions(commonEntry.dlp);
    fillFromRecoInteractions(commonEntry.pandora);
  }

  gROOT->SetBatch(kTRUE);
  TCanvas canvas("cMuonEnergyResolution", "Muon energy resolution", 900, 600);
  resolutionHistogram.SetLineWidth(2);
  resolutionHistogram.Draw();
  canvas.SaveAs(outputImage.c_str());

  std::cout << "Wrote muon energy resolution histogram to " << outputImage
            << " with " << nFilled << " entries\n";
  return 0;
}
