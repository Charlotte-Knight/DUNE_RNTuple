import uproot
import matplotlib.pyplot as plt
import sys
import awkward as ak

f = uproot.open(sys.argv[1])
caf = f["CAF"].arrays()

ixn = ak.flatten(caf.common.dlp)
part = ak.flatten(ixn.part)

is_muon = abs(part.pdg) == 13
has_truth_match = ak.count(part.truth, axis=-1) > 0
reco_muons = part[is_muon & has_truth_match]

truth_ixn = ak.flatten(caf.mc.interactions)
true_part = ak.flatten(truth_ixn.part)

true_muon_idx = reco_muons.truth[:, 0] # assuming ordered by truthOverlap
true_muons = true_part[true_muon_idx]

resolution = (reco_muons.E - true_muons.E) / true_muons.E

plt.hist(resolution, bins=50, range=(-1, 1))
plt.xlabel("Muon Energy Resolution")
plt.ylabel("Counts")
plt.savefig("muon_energy_resolution.png")