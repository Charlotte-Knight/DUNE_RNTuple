#include <iostream>
#include <string>

#include <ROOT/RNTupleReader.hxx>
#include <TCanvas.h>
#include <TH1D.h>
#include <TROOT.h>
#include <TFile.h>

#include "Schema.h"

#include <chrono>
#include <iostream>

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills.root";
  const std::string dlpOutputImage =
      (argc > 2) ? argv[2] : "build/track_lengths_dlp.png";

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<common>("common");

  TH1D dlpHistogram("hTrackLengthDlp", "DLP track lengths;Length;Particles", 100,
                    0.0, 300.0);
  
  const auto loopStart = std::chrono::steady_clock::now();

  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    for (const auto &interaction : commonEntry.dlp) {
      for (const auto &particle : interaction.part) {
        dlpHistogram.Fill((particle.end - particle.start).R());
      }
    }
  }
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;

  return 0;
}
