import uproot
import matplotlib.pyplot as plt
import sys
import awkward as ak

def get_lengths(part):
  dx = part.end.fCoordinates.fX - part.start.fCoordinates.fX 
  dy = part.end.fCoordinates.fY - part.start.fCoordinates.fY 
  dz = part.end.fCoordinates.fZ - part.start.fCoordinates.fZ 
  return (dx**2 + dy**2 + dz**2)**0.5

def plot(feature, label):
  plt.hist(ak.flatten(feature, axis=None), bins=50)
  plt.xlabel(label)
  plt.ylabel("Frequency")
  plt.savefig(label + ".png")
  plt.clf()

f = uproot.open(sys.argv[1])
caf = f["CAF"].arrays()

plot(get_lengths(caf.common.dlp.part), "Track Length (dlp)")
plot(get_lengths(caf.common.pandora.part), "Track Length (pandora)")

plot(caf.common.dlp.Enu, "Enu (dlp)")
plot(caf.common.pandora.Enu, "Enu (pandora)")