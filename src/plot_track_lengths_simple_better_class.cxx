#include <chrono>
#include <iostream>
#include <string>

#include <ROOT/RNTupleReader.hxx>
#include <TH1D.h>

#include "ClassSchema.h"

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills_class_schema.root";
  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto dlpView = reader->GetCollectionView("common.dlp");
  auto particles = dlpView.GetCollectionView("_0.part");
  auto startView = particles.GetView<classschema::V3>("_0.start");
  auto endView = particles.GetView<classschema::V3>("_0.end");

  TH1D histogram("hTrackLengthDlp", "DLP track lengths;Length;Particles", 100,
                 0.0, 300.0);

  const auto loopStart = std::chrono::steady_clock::now();
  for (auto entryId : reader->GetEntryRange()) {
    for (auto interactionId : dlpView.GetCollectionRange(entryId)) {
      for (auto particleId : particles.GetCollectionRange(interactionId)) {
        histogram.Fill((endView(particleId) - startView(particleId)).Mag());
      }
    }
  }
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;
  return 0;
}
