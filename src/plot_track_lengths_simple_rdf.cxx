#include <chrono>
#include <iostream>
#include <string>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RNTupleDS.hxx>
#include <TH1D.h>

#include "TrackLengths.h"

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills.root";

  auto dataframe = ROOT::RDF::FromRNTuple("CAF", inputPath);
  auto columns = dataframe.Define(
      "dlpTrackLengths", TrackLengths,
      {"common.dlp.part.start", "common.dlp.part.end"});
  auto dlpHistogram = columns.Histo1D<RVec<double>>(
      {"hTrackLengthDlp", "DLP track lengths;Length;Particles", 100, 0.0, 300.0},
      "dlpTrackLengths");

  const auto loopStart = std::chrono::steady_clock::now();
  dlpHistogram.GetValue();
  const auto loopEnd = std::chrono::steady_clock::now();
  const std::chrono::duration<double> loopDuration = loopEnd - loopStart;
  std::cout << "Event-processing loop execution time: "
            << loopDuration.count() << " s" << std::endl;

  return 0;
}
