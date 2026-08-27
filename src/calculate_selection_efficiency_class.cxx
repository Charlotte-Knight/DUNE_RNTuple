#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <ROOT/RNTupleReader.hxx>

#include "ClassSchema.h"

namespace {
struct SelectionCounters {
  std::uint64_t totalInteractions = 0;
  std::uint64_t interactionsWithPrimary = 0;
  std::uint64_t passed = 0;
};

bool PassesSelection(const classschema::Interaction &interaction) {
  if (interaction.Enu <= 2.f) {
    return false;
  }

  bool hasPrimary = false;
  for (const auto &particle : interaction.part) {
    if (!particle.primary) {
      continue;
    }
    hasPrimary = true;
    if (!particle.contained) {
      return false;
    }
  }
  return hasPrimary;
}

void Accumulate(const std::vector<classschema::Interaction> &interactions,
                SelectionCounters &counters) {
  for (const auto &interaction : interactions) {
    ++counters.totalInteractions;

    bool hasPrimary = false;
    for (const auto &particle : interaction.part) {
      if (particle.primary) {
        hasPrimary = true;
        break;
      }
    }
    if (hasPrimary) {
      ++counters.interactionsWithPrimary;
    }

    if (PassesSelection(interaction)) {
      ++counters.passed;
    }
  }
}

void PrintReport(const std::string &label, const SelectionCounters &counters) {
  const double efficiencyTotal =
      counters.totalInteractions > 0
          ? static_cast<double>(counters.passed) /
                static_cast<double>(counters.totalInteractions)
          : 0.0;
  const double efficiencyPrimary =
      counters.interactionsWithPrimary > 0
          ? static_cast<double>(counters.passed) /
                static_cast<double>(counters.interactionsWithPrimary)
          : 0.0;

  std::cout << label << ":\n";
  std::cout << "  Total interactions: " << counters.totalInteractions << '\n';
  std::cout << "  Interactions with >=1 primary: "
            << counters.interactionsWithPrimary << '\n';
  std::cout << "  Passed selection: " << counters.passed << '\n';
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "  Efficiency (passed / total): " << efficiencyTotal << '\n';
  std::cout << "  Efficiency (passed / with-primary): " << efficiencyPrimary
            << '\n';
}
}  // namespace

int main(int argc, char **argv) {
  const std::string inputPath =
      (argc > 1) ? argv[1] : "build/example_spills_class_schema.root";

  auto reader = ROOT::RNTupleReader::Open("CAF", inputPath);
  auto commonView = reader->GetView<classschema::Common>("common");

  SelectionCounters dlpCounters;
  SelectionCounters pandoraCounters;

  for (auto entryId : reader->GetEntryRange()) {
    const auto commonEntry = commonView(entryId);
    Accumulate(commonEntry.dlp, dlpCounters);
    Accumulate(commonEntry.pandora, pandoraCounters);
  }

  PrintReport("DLP", dlpCounters);
  PrintReport("Pandora", pandoraCounters);

  return 0;
}
