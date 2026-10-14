#include <chrono>
#include <iostream>
#include <string>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RNTupleDS.hxx>
#include <TCanvas.h>
#include <TH1D.h>
#include <TROOT.h>

#include "TrackLengths.h"

namespace {
void SaveHistogram(TH1D &histogram, const char *canvasName,
                   const char *canvasTitle, const std::string &outputPath) {
  TCanvas canvas(canvasName, canvasTitle, 900, 600);
  histogram.SetLineWidth(2);
  histogram.Draw();
  canvas.SaveAs(outputPath.c_str());
}
}  // namespace

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills.root";
  const std::string dlpOutputImage =
      (argc > 2) ? argv[2] : "build/track_lengths_dlp_rdf.png";
  const std::string pandoraOutputImage =
      (argc > 3) ? argv[3] : "build/track_lengths_pandora_rdf.png";
  const std::string dlpEnuOutputImage =
      (argc > 4) ? argv[4] : "build/enu_dlp_rdf.png";
  const std::string pandoraEnuOutputImage =
      (argc > 5) ? argv[5] : "build/enu_pandora_rdf.png";

  auto dataframe = ROOT::RDF::FromRNTuple("CAF", inputPath);
  auto columns = dataframe
      .Define("dlpTrackLengths", TrackLengths,
              {"common.dlp.part.start", "common.dlp.part.end"})
      .Define("pandoraTrackLengths", TrackLengths,
              {"common.pandora.part.start", "common.pandora.part.end"});

  auto dlpHistogram = columns.Histo1D(
      {"hTrackLengthDlp", "DLP track lengths;Length;Particles", 100, 0.0, 300.0},
      "dlpTrackLengths");
  auto pandoraHistogram = columns.Histo1D(
      {"hTrackLengthPandora", "Pandora track lengths;Length;Particles", 100, 0.0, 300.0},
      "pandoraTrackLengths");
  auto dlpEnuHistogram = columns.Histo1D(
      {"hEnuDlp", "DLP Enu;Enu;Particles", 100, 0.0, 10.0}, "common.dlp.Enu");
  auto pandoraEnuHistogram = columns.Histo1D(
      {"hEnuPandora", "Pandora Enu;Enu;Particles", 100, 0.0, 10.0},
      "common.pandora.Enu");

  // All actions are booked before triggering the shared event loop.
  const auto loopStart = std::chrono::steady_clock::now();
  dlpHistogram.GetValue();
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;

  gROOT->SetBatch(kTRUE);
  SaveHistogram(*dlpHistogram, "cTrackLengthDlp", "DLP track lengths",
                dlpOutputImage);
  SaveHistogram(*pandoraHistogram, "cTrackLengthPandora", "Pandora track lengths",
                pandoraOutputImage);
  SaveHistogram(*dlpEnuHistogram, "cEnuDlp", "DLP Enu", dlpEnuOutputImage);
  SaveHistogram(*pandoraEnuHistogram, "cEnuPandora", "Pandora Enu",
                pandoraEnuOutputImage);

  std::cout << "Wrote DLP histogram image to " << dlpOutputImage << '\n';
  std::cout << "Wrote Pandora histogram image to " << pandoraOutputImage << '\n';
  std::cout << "Wrote DLP Enu histogram image to " << dlpEnuOutputImage << '\n';
  std::cout << "Wrote Pandora Enu histogram image to " << pandoraEnuOutputImage
            << '\n';
  return 0;
}
