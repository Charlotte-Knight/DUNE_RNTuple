# DUNE_RNTuple

This repository is a playground for an RNTuple replacement for the StandardRecord CAFs. 

I originally thought that the CAF schema would have to become for native-type-like for things to play nicely with RNTuple. It turns out that it is possible to convert directly from a TTree CAF to an RNTuple CAF as-is. I've written a simple tool to perform this conversion that you can find [here](https://github.com/Charlotte-Knight/CAFtoRNTuple).

Regardless of the above, I am still finding this repo useful as a starting point for exploring RNTuple and RDataFrame ideas with a CAF-like structure.

You can follow the instructions below or start exploring `example_spills.root`.

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build -j
```

## Generate an example RNTuple file

Run the writer executable:

```bash
./build/write_example_rntuple ./build/example_spills.root
```

This writes one RNTuple named `CAF` containing the `common` (reco) and `mc` (truth) fields.

## Generate an example RNTuple file with the class-based schema

Run the alternative writer executable:

```bash
./build/write_example_class_rntuple ./build/example_spills_class_schema.root
```

This writes the same top-level `common` and `mc` fields, but uses
[`ClassSchema.h`](./include/ClassSchema.h)'s `classschema::*` classes instead of
the struct-based schema. The custom `classschema::V3` exposes methods like
`.Mag()` and `.Unit()`.

Both writers use the shared spill count (1,300) and random seed from
[`ExampleConfig.h`](./include/ExampleConfig.h). Regenerate any older
class-based sample: the previous writer produced only 13 spills.
The matching sample sizes are verified by the comparison test below.

To verify readback and exercise those methods after deserialization:

```bash
./build/summarize_class_vectors ./build/example_spills_class_schema.root
```

## Plot track lengths and Enu for DLP and Pandora

There are five approaches, each with struct- and class-based versions.
All fill one length per particle, without requiring
`recoType == RecoObjType::kTrack`. The full plotters also fill one Enu per
interaction; the simple versions fill only DLP lengths and do not save images.

### Versions and timings

The following are historical measurements, before adding the matching-size
class-based samples and the other class-based plotters.
Measured on 2026-10-15 with an Apple M5 (arm64), ROOT 6.40.04, and the existing
CMake build with an empty `CMAKE_BUILD_TYPE` (no optimization flag). Times are
medians of three fresh-process runs after one discarded warm-up run per
executable, with filesystem caches left warm and implicit multithreading not
enabled.

| Version | Data deserialized / work performed | Processing time | Total wall time |
|---|---|---:|---:|
| [`plot_track_lengths`](./src/plot_track_lengths.cxx) | Full `common` object; four DLP/Pandora length and Enu histograms | 0.644 s | 0.909 s |
| [`plot_track_lengths_rdf`](./src/plot_track_lengths_rdf.cxx) | DLP/Pandora endpoints and Enu; four histograms in one RDF event loop | 0.800 s | 1.082 s |
| [`plot_track_lengths_simple`](./src/plot_track_lengths_simple.cxx) | Full `common` object; only DLP lengths used | 0.639 s | 0.824 s |
| [`plot_track_lengths_simple_better`](./src/plot_track_lengths_simple_better.cxx) | DLP endpoints through nested collection views; DLP lengths only | 0.033 s | 0.196 s |
| [`plot_track_lengths_simple_rdf`](./src/plot_track_lengths_simple_rdf.cxx) | DLP endpoints through RDF; DLP lengths only, explicitly typed histogram action | 0.027 s | 0.199 s |
| [`plot_track_lengths_class`](./src/plot_track_lengths_class.cxx) | Full class-based `Common` object; four histograms using `Particle::TrackLength()` | Not instrumented | 0.296 s* |

The five struct-based versions used `build/example_spills.root` (about 74 MiB).
*The class-based version used the separate, much smaller
`build/example_spills_class_schema.root` (about 809 KiB), so its time is not
directly comparable.*

Processing time is the executable's printed timer around the event loop (or
RDF's first `GetValue()`); it excludes reader/view construction and image
output. Total wall time includes process startup, setup, processing, and, for
the full plotters, drawing and saving all four PNGs. These are illustrative
local measurements, not performance guarantees.

The full RDF version currently uses type-inferred `Histo1D` calls, so its
processing time includes JIT compilation of the histogram actions. The simple
RDF version uses `Histo1D<RVec<double>>`, compiling the action ahead of time.
Consequently, the full RDF timing is not a pure measure of endpoint I/O.
The vector-valued column still produces one fill per particle, not one per
spill. For optimized comparisons, build all variants consistently with
`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` and repeat the measurements.

### Full RNTupleReader version

Run the plotting executable:

```bash
./build/plot_track_lengths \
  ./build/example_spills.root \
  ./build/track_lengths_dlp.png \
  ./build/track_lengths_pandora.png \
  ./build/enu_dlp.png \
  ./build/enu_pandora.png
```

This produces four PNG files:

- `build/track_lengths_dlp.png`
- `build/track_lengths_pandora.png`
- `build/enu_dlp.png`
- `build/enu_pandora.png`

### RDataFrame version

The RDataFrame equivalent uses `ROOT::RDF::FromRNTuple` and books all four
histograms before running a single event loop:

```bash
./build/plot_track_lengths_rdf \
  ./build/example_spills.root \
  ./build/track_lengths_dlp_rdf.png \
  ./build/track_lengths_pandora_rdf.png \
  ./build/enu_dlp_rdf.png \
  ./build/enu_pandora_rdf.png
```

It uses the same binning and fills as `plot_track_lengths`: one length per
particle (without a track-type selection) and one Enu per interaction.
Only the required endpoint and Enu columns are read, rather than the full
`common` object. The reported processing time includes RDataFrame's first-run
setup/JIT overhead, but excludes drawing and saving the images.
All output arguments are optional; their defaults are the paths shown above.

### Simple timing-only versions

The baseline reads the full `common` object but uses only DLP particles:

```bash
./build/plot_track_lengths_simple ./build/example_spills.root
```

For its RDataFrame equivalent, run:

```bash
./build/plot_track_lengths_simple_rdf ./build/example_spills.root
```

This fills only the DLP particle-length histogram, with the same binning as
the original, and prints the processing time without drawing or saving an
image. It reads only DLP endpoint columns and explicitly specifies the
`RVec<double>` histogram input type to avoid type-inference JIT compilation.
The timed region still includes first-run RDF event-loop initialization.

For an endpoint-only `RNTupleReader` version, run:

```bash
./build/plot_track_lengths_simple_better ./build/example_spills.root
```

This uses collection views of `common.dlp` and its `_0.part` subfield to
translate spill indices into interaction indices and then particle indices.
`_0` denotes a collection's element field, not its first element.
Typed `V3` views of `_0.start` and `_0.end` then deserialize only the endpoints;
the collection views read the offsets needed to traverse the nesting.
No enclosing `common`, `Interaction`, or full `Particle` objects are read.
Like the other simple versions, it fills a histogram and reports processing
time without saving an image.

### Class-based version

Run the class-based variant using the `ClassSchema.h` types:

```bash
./build/plot_track_lengths_class \
  ./build/example_spills_class_schema.root \
  ./build/track_lengths_dlp_class.png \
  ./build/track_lengths_pandora_class.png \
  ./build/enu_dlp_class.png \
  ./build/enu_pandora_class.png
```

This uses the custom `classschema::Particle::TrackLength()` method to compute track lengths.

Every struct-based approach now has a class-based counterpart:

| Approach | Class-based executable |
|---|---|
| Full reader; DLP/Pandora lengths and Enu, four images | [`plot_track_lengths_class`](./src/plot_track_lengths_class.cxx) |
| Full RDF; the same four histograms/images, type-inferred actions | [`plot_track_lengths_rdf_class`](./src/plot_track_lengths_rdf_class.cxx) |
| Full `Common` view; DLP lengths only | [`plot_track_lengths_simple_class`](./src/plot_track_lengths_simple_class.cxx) |
| Endpoint-only collection views; DLP lengths only | [`plot_track_lengths_simple_better_class`](./src/plot_track_lengths_simple_better_class.cxx) |
| Endpoint-only RDF; DLP lengths only, typed action | [`plot_track_lengths_simple_rdf_class`](./src/plot_track_lengths_simple_rdf_class.cxx) |

All class-based executables default to
`build/example_spills_class_schema.root` and print the processing time.
The three simple variants do not draw or save images. The full RDF variant
accepts the same four optional output-image arguments as the full reader,
defaulting to `build/track_lengths_{dlp,pandora}_rdf_class.png` and
`build/enu_{dlp,pandora}_rdf_class.png`.
RDF reads nested endpoint columns as `RVec`s even though the class schema uses
`std::vector` collections.

For reproducible optimized comparisons, build and generate matching inputs:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build-release -j
ctest --test-dir build-release --output-on-failure
```

The tests generate `build-release/comparison_struct.root` and
`build-release/comparison_class.root`. Both contain 1,300 spills, 64,984 DLP
interactions, 66,420 Pandora interactions, and 130,090 truth interactions.
They also contain identical per-spill/per-interaction collection sizes and
matching histogram contents. The test checks all length/Enu histogram bins and
errors, including underflow/overflow, across schemas and between full-object,
endpoint-view, and RDF reads. To rerun the comparison on other generated files:

```bash
./build-release/compare_track_length_schemas STRUCT_FILE CLASS_FILE
```

For example, compare the endpoint-only variants on the tested inputs:

```bash
./build-release/plot_track_lengths_simple_better \
  ./build-release/comparison_struct.root
./build-release/plot_track_lengths_simple_better_class \
  ./build-release/comparison_class.root
```

Use matching build flags and workload for each pair. This compares the complete
schemas, not just the `class` versus `struct` keywords: the class schema uses
`std::vector` and a custom `V3`, whereas the struct schema uses `RVec` and
`ROOT::Math::XYZVector`.

## Plot muon energy resolution

Run:

```bash
./build/plot_muon_energy_resolution \
  ./build/example_spills.root \
  ./build/muon_energy_resolution.png
```

This plots `(E_{reco} - E_{true}) / E_{true}` for muons using truth-matched
reco particles across DLP and Pandora interactions.

### Class-based version

Run:

```bash
./build/plot_muon_energy_resolution_class \
  ./build/example_spills_class_schema.root \
  ./build/muon_energy_resolution_class.png
```

## Calculate Enu/containment selection efficiency

Run:

```bash
./build/calculate_selection_efficiency ./build/example_spills.root
```

Selection used per interaction:

- interaction-level `Enu > 2`
- all primary particles are contained

The script prints efficiency summaries for both DLP and Pandora.

### Class-based version

Run:

```bash
./build/calculate_selection_efficiency_class ./build/example_spills_class_schema.root
```

Produces identical selection efficiency output using the class-based schema.

## RBrowser

You can explore the RNTuple fills in a TBrowser and it'll look like this.

![tbrowser visualation of the rntuple](tbrowser.png)

By the way, there is a RBrowser [extension](https://marketplace.visualstudio.com/items?itemName=CharlotteKnight.root-rbrowser) in vscode that you can try.

## Next plans

Try and do the following things using this RNTuple:

1. Filling a histogram with a particle-level quantity
2. Filling a histogram with an interaction-level quantity
3. Applying a selection and calculating an efficiency
4. Making an energy resolution plot (tests truth matching)
5. Testing schema evolution

Attempting this in a variety of frameworks, with C++ being primary customers, and then Python-based approaches, e.g. uproot.

More ideas are welcome.