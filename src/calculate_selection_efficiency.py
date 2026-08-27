import uproot
import matplotlib.pyplot as plt
import sys
import awkward as ak

f = uproot.open(sys.argv[1])
caf = f["CAF"].arrays()

def passes_selection(ixn):
  enu_cut = ixn.Enu > 2
  has_primary = ak.any(ixn.part.primary, axis=-1)
  all_primary_contained = ak.all(ixn.part.contained | ~ixn.part.primary, axis=-1)

  return enu_cut & has_primary & all_primary_contained

ixn = ak.flatten(caf.common.dlp)
selection_mask = passes_selection(ixn)

efficiency = ak.sum(selection_mask) / len(selection_mask)
print(f"Selection efficiency: {efficiency:.4f}")