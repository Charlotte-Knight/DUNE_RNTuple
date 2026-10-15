#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RNTupleDS.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <TH1D.h>

#include "ClassTrackLengths.h"
#include "ExampleConfig.h"
#include "TrackLengths.h"

namespace {
void Require(bool condition, const std::string &message) {
  if (!condition)
    throw std::runtime_error(message);
}

void CompareHistograms(const TH1D &actual, const TH1D &expected) {
  Require(actual.GetEntries() == expected.GetEntries(), "Histogram entry counts differ");
  for (int bin = 0; bin <= expected.GetNbinsX() + 1; ++bin) {
    Require(actual.GetBinContent(bin) == expected.GetBinContent(bin),
            "Histogram bin contents differ");
    Require(actual.GetBinError(bin) == expected.GetBinError(bin),
            "Histogram bin errors differ");
  }
}

struct Result {
  // Spills, DLP/Pandora/truth interactions, DLP/Pandora/truth particles.
  std::array<std::uint64_t, 7> counts{};
  std::vector<std::size_t> collectionSizes;
  std::array<TH1D, 4> histograms{
      TH1D("dlp", "", 100, 0., 300.), TH1D("pandora", "", 100, 0., 300.),
      TH1D("dlpEnu", "", 100, 0., 10.), TH1D("pandoraEnu", "", 100, 0., 10.)};
};

template <typename Common, typename Mc, typename Vector, typename Length>
Result Check(const std::string &path, Length length,
             ROOT::VecOps::RVec<double> (*trackLengths)(
                 const ROOT::VecOps::RVec<ROOT::VecOps::RVec<Vector>> &,
                 const ROOT::VecOps::RVec<ROOT::VecOps::RVec<Vector>> &)) {
  Result result;
  auto reader = ROOT::RNTupleReader::Open("CAF", path);
  auto commonView = reader->GetView<Common>("common");
  auto mcView = reader->GetView<Mc>("mc");
  auto dlp = reader->GetCollectionView("common.dlp");
  auto particles = dlp.GetCollectionView("_0.part");
  auto starts = particles.GetView<Vector>("_0.start");
  auto ends = particles.GetView<Vector>("_0.end");
  TH1D endpointHistogram("endpoints", "", 100, 0., 300.);
  for (auto entryId : reader->GetEntryRange()) {
    ++result.counts[0];
    const auto entry = commonView(entryId);
    const auto truth = mcView(entryId);
    auto accumulate = [&](const auto &interactions, std::size_t index) {
      result.collectionSizes.push_back(interactions.size());
      result.counts[1 + index] += interactions.size();
      for (const auto &interaction : interactions) {
        result.collectionSizes.push_back(interaction.part.size());
        result.counts[4 + index] += interaction.part.size();
        if (index < 2) {
          result.histograms[2 + index].Fill(interaction.Enu);
          for (const auto &particle : interaction.part)
            result.histograms[index].Fill(length(particle.end - particle.start));
        }
      }
    };
    accumulate(entry.dlp, 0);
    accumulate(entry.pandora, 1);
    accumulate(truth.interactions, 2);
    for (auto interactionId : dlp.GetCollectionRange(entryId))
      for (auto particleId : particles.GetCollectionRange(interactionId))
        endpointHistogram.Fill(length(ends(particleId) - starts(particleId)));
  }
  CompareHistograms(endpointHistogram, result.histograms[0]);

  auto dataframe = ROOT::RDF::FromRNTuple("CAF", path);
  auto columns = dataframe
      .Define("dlpLengths", trackLengths, {"common.dlp.part.start", "common.dlp.part.end"})
      .Define("pandoraLengths", trackLengths,
              {"common.pandora.part.start", "common.pandora.part.end"});
  std::array<ROOT::RDF::RResultPtr<TH1D>, 4> rdfHistograms{
      columns.template Histo1D<ROOT::VecOps::RVec<double>>(
          {"rdfDlp", "", 100, 0., 300.}, "dlpLengths"),
      columns.template Histo1D<ROOT::VecOps::RVec<double>>(
          {"rdfPandora", "", 100, 0., 300.}, "pandoraLengths"),
      columns.template Histo1D<ROOT::VecOps::RVec<float>>(
          {"rdfDlpEnu", "", 100, 0., 10.}, "common.dlp.Enu"),
      columns.template Histo1D<ROOT::VecOps::RVec<float>>(
          {"rdfPandoraEnu", "", 100, 0., 10.}, "common.pandora.Enu")};
  for (std::size_t i = 0; i < rdfHistograms.size(); ++i)
    CompareHistograms(*rdfHistograms[i], result.histograms[i]);
  Require(dataframe.GetNRuns() == 1, "Expected one shared RDF event loop");
  return result;
}
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " STRUCT_FILE CLASS_FILE\n";
    return 1;
  }
  TH1::AddDirectory(false);
  const auto structs = Check<common, mc, V3>(
      argv[1], [](const V3 &v) { return v.R(); }, TrackLengths);
  const auto classes = Check<classschema::Common, classschema::Mc, classschema::V3>(
      argv[2], [](const classschema::V3 &v) { return v.Mag(); }, classschema::TrackLengths);
  Require(structs.counts[0] == example::kSpillCount, "Unexpected spill count");
  Require(structs.counts == classes.counts, "Schema interaction/particle counts differ");
  Require(structs.collectionSizes == classes.collectionSizes,
          "Per-spill or per-interaction collection sizes differ");
  for (std::size_t i = 0; i < structs.histograms.size(); ++i)
    CompareHistograms(classes.histograms[i], structs.histograms[i]);
  const std::array<const char *, 7> labels{
      "Spills", "DLP interactions", "Pandora interactions", "Truth interactions",
      "DLP particles", "Pandora particles", "Truth particles"};
  for (std::size_t i = 0; i < labels.size(); ++i)
    std::cout << labels[i] << ": " << structs.counts[i] << '\n';
  std::cout << "Per-spill counts and all histogram bins/errors match across schemas, "
               "endpoint views, and RDF (including underflow/overflow).\n";
}
