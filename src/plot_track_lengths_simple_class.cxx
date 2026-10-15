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
  auto commonView = reader->GetView<classschema::Common>("common");

  TH1D histogram("hTrackLengthDlp", "DLP track lengths;Length;Particles", 100,
                 0.0, 300.0);

  const auto loopStart = std::chrono::steady_clock::now();
  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    for (const auto &interaction : commonEntry.dlp) {
      for (const auto &particle : interaction.part) {
        histogram.Fill(particle.TrackLength());
      }
    }
  }
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;
  return 0;
}
