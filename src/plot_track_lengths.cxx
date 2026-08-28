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
  const std::string pandoraOutputImage =
      (argc > 3) ? argv[3] : "build/track_lengths_pandora.png";
  const std::string dlpEnuOutputImage =
      (argc > 4) ? argv[4] : "build/enu_dlp.png";
  const std::string pandoraEnuOutputImage =
      (argc > 5) ? argv[5] : "build/enu_pandora.png";

  TFile f(inputPath.c_str());
  f.GetStreamerInfoList()->ls();  // shows versions/checksums ROOT stored

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<common>("common");

  TH1D dlpHistogram("hTrackLengthDlp", "DLP track lengths;Length;Particles", 100,
                    0.0, 300.0);
  TH1D pandoraHistogram("hTrackLengthPandora",
                        "Pandora track lengths;Length;Particles", 100, 0.0,
                        300.0);
  TH1D dlpEnuHistogram("hEnuDlp", "DLP Enu;Enu;Particles", 100, 0.0, 10.0);
  TH1D pandoraEnuHistogram("hEnuPandora", "Pandora Enu;Enu;Particles", 100, 0.0,
                           10.0);

  const auto loopStart = std::chrono::steady_clock::now();

  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    for (const auto &interaction : commonEntry.dlp) {
      dlpEnuHistogram.Fill(interaction.Enu);
      for (const auto &particle : interaction.part) {
        dlpHistogram.Fill((particle.end - particle.start).R());
      }
    }
    for (const auto &interaction : commonEntry.pandora) {
      pandoraEnuHistogram.Fill(interaction.Enu);
      for (const auto &particle : interaction.part) {
        pandoraHistogram.Fill((particle.end - particle.start).R());
      }
    }
  }
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;

  gROOT->SetBatch(kTRUE);
  TCanvas dlpCanvas("cTrackLengthDlp", "DLP track lengths", 900, 600);
  dlpHistogram.SetLineWidth(2);
  dlpHistogram.Draw();
  dlpCanvas.SaveAs(dlpOutputImage.c_str());

  TCanvas pandoraCanvas("cTrackLengthPandora", "Pandora track lengths", 900, 600);
  pandoraHistogram.SetLineWidth(2);
  pandoraHistogram.Draw();
  pandoraCanvas.SaveAs(pandoraOutputImage.c_str());

  TCanvas dlpEnuCanvas("cEnuDlp", "DLP Enu", 900, 600);
  dlpEnuHistogram.SetLineWidth(2);
  dlpEnuHistogram.Draw();
  dlpEnuCanvas.SaveAs(dlpEnuOutputImage.c_str());

  TCanvas pandoraEnuCanvas("cEnuPandora", "Pandora Enu", 900, 600);
  pandoraEnuHistogram.SetLineWidth(2);
  pandoraEnuHistogram.Draw();
  pandoraEnuCanvas.SaveAs(pandoraEnuOutputImage.c_str());

  std::cout << "Wrote DLP histogram image to " << dlpOutputImage << '\n';
  std::cout << "Wrote Pandora histogram image to " << pandoraOutputImage << '\n';
  std::cout << "Wrote DLP Enu histogram image to " << dlpEnuOutputImage << '\n';
  std::cout << "Wrote Pandora Enu histogram image to " << pandoraEnuOutputImage
            << '\n';
  return 0;
}
