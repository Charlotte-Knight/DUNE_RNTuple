#include <iostream>
#include <string>

#include <ROOT/RNTupleReader.hxx>
#include <TCanvas.h>
#include <TH1D.h>
#include <TROOT.h>

#include "Schema.h"

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills.root";
  const std::string dlpOutputImage =
      (argc > 2) ? argv[2] : "build/track_lengths_dlp.png";
  const std::string pandoraOutputImage =
      (argc > 3) ? argv[3] : "build/track_lengths_pandora.png";

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<common>("common");

  TH1D dlpHistogram("hTrackLengthDlp", "DLP track lengths;Length;Particles", 100,
                    0.0, 300.0);
  TH1D pandoraHistogram("hTrackLengthPandora",
                        "Pandora track lengths;Length;Particles", 100, 0.0,
                        300.0);

  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    for (const auto &interaction : commonEntry.dlp) {
      for (const auto &particle : interaction.part) {
        dlpHistogram.Fill((particle.end - particle.start).R());
      }
    }
    for (const auto &interaction : commonEntry.pandora) {
      for (const auto &particle : interaction.part) {
        pandoraHistogram.Fill((particle.end - particle.start).R());
      }
    }
  }

  gROOT->SetBatch(kTRUE);
  TCanvas dlpCanvas("cTrackLengthDlp", "DLP track lengths", 900, 600);
  dlpHistogram.SetLineWidth(2);
  dlpHistogram.Draw();
  dlpCanvas.SaveAs(dlpOutputImage.c_str());

  TCanvas pandoraCanvas("cTrackLengthPandora", "Pandora track lengths", 900, 600);
  pandoraHistogram.SetLineWidth(2);
  pandoraHistogram.Draw();
  pandoraCanvas.SaveAs(pandoraOutputImage.c_str());

  std::cout << "Wrote DLP histogram image to " << dlpOutputImage << '\n';
  std::cout << "Wrote Pandora histogram image to " << pandoraOutputImage << '\n';
  return 0;
}
