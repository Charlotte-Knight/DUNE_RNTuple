#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>

#include "Schema.h"

int main(int argc, char **argv) {
  const std::string outputPath = (argc > 1) ? argv[1] : "example_spills.root";

  auto model = ROOT::RNTupleModel::Create();
  auto commonField = model->MakeField<common>("common");
  auto mcField = model->MakeField<mc>("mc");
  auto writer = ROOT::RNTupleWriter::Recreate(std::move(model), "CAF",
                                              outputPath);

  std::mt19937_64 rng(0xD00E2026ULL);

  std::uniform_real_distribution<double> xyDist(-500.0, 500.0);
  std::uniform_real_distribution<double> zDist(0.0, 1200.0);
  std::uniform_real_distribution<double> dirDist(-1.0, 1.0);
  std::uniform_real_distribution<double> lengthDist(10.0, 350.0);
  std::normal_distribution<double> vertexSmearDist(0.0, 3.0);
  std::normal_distribution<double> startSmearDist(0.0, 1.5);
  std::normal_distribution<double> endSmearDist(0.0, 2.5);
  std::normal_distribution<double> momentumSmearFrac(0.0, 0.08);
  std::normal_distribution<double> energySmearFrac(0.0, 0.12);
  std::uniform_real_distribution<float> truthEnergyDist(0.05f, 8.0f);
  std::uniform_real_distribution<float> scoreDist(0.45f, 1.0f);
  std::uniform_real_distribution<float> overlapDist(0.65f, 0.99f);
  std::uniform_int_distribution<int> truthIntCountDist(90, 110);
  std::uniform_int_distribution<int> recoMatchedCountDist(42, 52);
  std::uniform_int_distribution<int> truthPartCountDist(1, 7);
  std::uniform_int_distribution<int> tgtADist(0, 40);
  std::uniform_int_distribution<int> fakePartCountDist(1, 6);
  std::bernoulli_distribution isPrimaryDist(0.4);
  std::bernoulli_distribution containsDist(0.6);
  std::bernoulli_distribution misIdDist(0.08);
  std::bernoulli_distribution recoParticleEffDist(0.88);
  std::bernoulli_distribution secondTruthLinkDist(0.10);
  std::bernoulli_distribution particleSecondTruthLinkDist(0.05);
  std::poisson_distribution<int> fakeDlpInteractionsDist(3.0);
  std::poisson_distribution<int> fakePandoraInteractionsDist(4.0);
  std::array<int, 8> pdgChoices{{13, -13, 211, -211, 2212, 11, -11, 22}};
  std::uniform_int_distribution<int> pdgChoiceDist(
      0, static_cast<int>(pdgChoices.size() - 1));

  auto randomPosition = [&]() { return V3(xyDist(rng), xyDist(rng), zDist(rng)); };
  auto randomDirection = [&]() {
    V3 direction;
    do {
      direction = V3(dirDist(rng), dirDist(rng), dirDist(rng));
    } while (direction.R() < 1e-6);
    return direction.Unit();
  };
  auto smearVector = [&](const V3 &value, std::normal_distribution<double> &dist) {
    return V3(value.X() + dist(rng), value.Y() + dist(rng), value.Z() + dist(rng));
  };
  auto choosePdg = [&]() { return pdgChoices[pdgChoiceDist(rng)]; };
  auto chooseRecoType = [](int pdg) {
    const int absPdg = std::abs(pdg);
    if (absPdg == 11 || absPdg == 22) {
      return RecoObjType::kShower;
    }
    return RecoObjType::kTrack;
  };
  auto chooseEMethod = [](RecoObjType recoType) {
    if (recoType == RecoObjType::kShower) {
      return PartEMethod::kCalorimetry;
    }
    return PartEMethod::kRange;
  };

  std::uint64_t nextRecoInteractionId = 1;
  std::uint64_t nextRecoParticleId = 1;
  std::uint64_t nextTruthInteractionId = 1;
  std::uint64_t nextTruthParticleId = 1;

  for (std::uint64_t spillIndex = 0; spillIndex < 13; ++spillIndex) {
    common commonEntry;
    commonEntry.id = 1000 + spillIndex;

    mc mcEntry;
    mcEntry.id = commonEntry.id;

    const int nTruthInteractions = truthIntCountDist(rng);
    mcEntry.interactions.reserve(static_cast<std::size_t>(nTruthInteractions));

    std::vector<std::uint64_t> truthInteractionIds;
    truthInteractionIds.reserve(static_cast<std::size_t>(nTruthInteractions));
    std::vector<std::uint64_t> truthParticleIds;
    truthParticleIds.reserve(static_cast<std::size_t>(nTruthInteractions * 4));

    for (int interactionIndex = 0; interactionIndex < nTruthInteractions;
         ++interactionIndex) {
      TruthInteraction truthInteraction;
      truthInteraction.id = nextTruthInteractionId++;
      truthInteraction.vtx = randomPosition();

      const int nTruthParticles = truthPartCountDist(rng);
      truthInteraction.part.reserve(static_cast<std::size_t>(nTruthParticles));
      for (int particleIndex = 0; particleIndex < nTruthParticles;
           ++particleIndex) {
        TruthParticle truthParticle;
        truthParticle.id = nextTruthParticleId++;
        truthParticle.primary = (particleIndex == 0) || isPrimaryDist(rng);
        truthParticle.pdg = choosePdg();
        truthParticle.tgtA = tgtADist(rng);
        truthParticle.E = truthEnergyDist(rng);

        const V3 direction = randomDirection();
        const double momentumMag =
            std::max(0.05, static_cast<double>(truthParticle.E) * 0.8);
        truthParticle.p = direction * momentumMag;
        truthParticle.start = truthInteraction.vtx + randomDirection() * 5.0;
        truthParticle.end = truthParticle.start + direction * lengthDist(rng);

        truthInteraction.part.push_back(truthParticle);
        truthParticleIds.push_back(truthParticle.id);
      }

      truthInteractionIds.push_back(truthInteraction.id);
      mcEntry.interactions.push_back(truthInteraction);
    }

    std::unordered_map<std::uint64_t, std::vector<std::pair<std::uint64_t, float>>>
        truthRecoLinks;

    auto buildRecoFromTruth = [&](const TruthInteraction &truthInteraction) {
      Interaction recoInteraction;
      recoInteraction.id = nextRecoInteractionId++;
      recoInteraction.vtx = smearVector(truthInteraction.vtx, vertexSmearDist);
      recoInteraction.truth.push_back(truthInteraction.id);
      recoInteraction.truthOverlap.push_back(overlapDist(rng));

      if (truthInteractionIds.size() > 1 && secondTruthLinkDist(rng)) {
        std::uniform_int_distribution<std::size_t> truthPick(
            0, truthInteractionIds.size() - 1);
        const auto extraTruthId = truthInteractionIds[truthPick(rng)];
        if (extraTruthId != truthInteraction.id) {
          recoInteraction.truth.push_back(extraTruthId);
          recoInteraction.truthOverlap.push_back(overlapDist(rng) * 0.6f);
        }
      }

      for (std::size_t truthLinkIdx = 0; truthLinkIdx < recoInteraction.truth.size();
           ++truthLinkIdx) {
        truthRecoLinks[recoInteraction.truth[truthLinkIdx]].push_back(
            {recoInteraction.id, recoInteraction.truthOverlap[truthLinkIdx]});
      }

      recoInteraction.part.reserve(truthInteraction.part.size());
      for (const auto &truthParticle : truthInteraction.part) {
        if (!recoParticleEffDist(rng)) {
          continue;
        }

        Particle recoParticle;
        recoParticle.id = nextRecoParticleId++;
        recoParticle.primary = truthParticle.primary;
        recoParticle.pdg =
            misIdDist(rng) ? choosePdg() : static_cast<int>(truthParticle.pdg);
        recoParticle.tgtA = truthParticle.tgtA;
        recoParticle.score = scoreDist(rng);

        const double energyScale = 1.0 + energySmearFrac(rng);
        recoParticle.E = static_cast<float>(
            std::max(0.02, static_cast<double>(truthParticle.E) * energyScale));

        const double momentumScale = 1.0 + momentumSmearFrac(rng);
        recoParticle.p = truthParticle.p * momentumScale;
        recoParticle.start = smearVector(truthParticle.start, startSmearDist);
        recoParticle.end = smearVector(truthParticle.end, endSmearDist);
        recoParticle.contained = containsDist(rng);
        recoParticle.walldist =
            std::max(0.0f, static_cast<float>(200.0 - std::abs(recoParticle.end.Z() - 600.0)));
        recoParticle.recoType = chooseRecoType(recoParticle.pdg);
        recoParticle.EMethod = chooseEMethod(recoParticle.recoType);
        recoParticle.truth.push_back(truthParticle.id);
        recoParticle.truthOverlap.push_back(overlapDist(rng));

        if (truthParticleIds.size() > 1 && particleSecondTruthLinkDist(rng)) {
          std::uniform_int_distribution<std::size_t> particleTruthPick(
              0, truthParticleIds.size() - 1);
          const auto extraTruthPartId = truthParticleIds[particleTruthPick(rng)];
          if (extraTruthPartId != truthParticle.id) {
            recoParticle.truth.push_back(extraTruthPartId);
            recoParticle.truthOverlap.push_back(overlapDist(rng) * 0.4f);
          }
        }

        recoInteraction.part.push_back(recoParticle);
      }

      return recoInteraction;
    };

    auto addFakeRecoInteractions = [&](std::vector<Interaction> &collection,
                                       int nFakes) {
      for (int fakeIndex = 0; fakeIndex < nFakes; ++fakeIndex) {
        Interaction fakeInteraction;
        fakeInteraction.id = nextRecoInteractionId++;
        fakeInteraction.vtx = randomPosition();
        const int nFakeParticles = fakePartCountDist(rng);
        fakeInteraction.part.reserve(static_cast<std::size_t>(nFakeParticles));

        for (int particleIndex = 0; particleIndex < nFakeParticles;
             ++particleIndex) {
          Particle fakeParticle;
          fakeParticle.id = nextRecoParticleId++;
          fakeParticle.primary = (particleIndex == 0) || isPrimaryDist(rng);
          fakeParticle.pdg = choosePdg();
          fakeParticle.tgtA = tgtADist(rng);
          fakeParticle.score = scoreDist(rng) * 0.7f;
          fakeParticle.E = truthEnergyDist(rng);
          fakeParticle.p = randomDirection() * static_cast<double>(fakeParticle.E);
          fakeParticle.start = fakeInteraction.vtx + randomDirection() * 8.0;
          fakeParticle.end = fakeParticle.start + randomDirection() * lengthDist(rng);
          fakeParticle.contained = containsDist(rng);
          fakeParticle.walldist =
              std::max(0.0f, static_cast<float>(200.0 - std::abs(fakeParticle.end.Z() - 600.0)));
          fakeParticle.recoType = chooseRecoType(fakeParticle.pdg);
          fakeParticle.EMethod = chooseEMethod(fakeParticle.recoType);
          fakeInteraction.part.push_back(fakeParticle);
        }

        collection.push_back(fakeInteraction);
      }
    };

    const int nDlpTruthMatched = std::min(
        recoMatchedCountDist(rng), static_cast<int>(mcEntry.interactions.size()));
    const int nPandoraTruthMatched =
        std::min(recoMatchedCountDist(rng),
                 static_cast<int>(mcEntry.interactions.size()));

    auto pickTruthIndices = [&](int nToPick) {
      std::vector<std::size_t> indices(mcEntry.interactions.size());
      std::iota(indices.begin(), indices.end(), 0);
      std::shuffle(indices.begin(), indices.end(), rng);
      indices.resize(static_cast<std::size_t>(nToPick));
      return indices;
    };

    const auto dlpTruthIndices = pickTruthIndices(nDlpTruthMatched);
    const auto pandoraTruthIndices = pickTruthIndices(nPandoraTruthMatched);

    commonEntry.dlp.reserve(static_cast<std::size_t>(nDlpTruthMatched));
    commonEntry.pandora.reserve(static_cast<std::size_t>(nPandoraTruthMatched));

    for (const auto index : dlpTruthIndices) {
      commonEntry.dlp.push_back(buildRecoFromTruth(mcEntry.interactions[index]));
    }
    for (const auto index : pandoraTruthIndices) {
      commonEntry.pandora.push_back(buildRecoFromTruth(mcEntry.interactions[index]));
    }

    addFakeRecoInteractions(commonEntry.dlp, fakeDlpInteractionsDist(rng));
    addFakeRecoInteractions(commonEntry.pandora, fakePandoraInteractionsDist(rng));

    for (auto &truthInteraction : mcEntry.interactions) {
      const auto foundLinks = truthRecoLinks.find(truthInteraction.id);
      if (foundLinks == truthRecoLinks.end()) {
        continue;
      }
      for (const auto &link : foundLinks->second) {
        truthInteraction.reco.push_back(link.first);
        truthInteraction.recoOverlap.push_back(link.second);
      }
    }

    *commonField = commonEntry;
    *mcField = mcEntry;
    writer->Fill();
  }

  return 0;
}
