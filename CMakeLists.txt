cmake_minimum_required(VERSION 3.16)
project(DUNE_RNTuple_Example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

set(SCHEMA_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/Schema.h)
set(LINKDEF_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h)
set(CLASS_SCHEMA_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/ClassSchema.h)
set(CLASS_LINKDEF_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/ClassLinkDef.h)

set(SCHEMA_DICT_DEPENDENCIES ROOT::ROOTVecOps)
if(TARGET VDT::VDT)
  # rootcling does not collect transitive include paths from dependencies.
  list(APPEND SCHEMA_DICT_DEPENDENCIES VDT::VDT)
endif()

ROOT_GENERATE_DICTIONARY(G__DuneSchema
  ${SCHEMA_HEADER}
  LINKDEF ${LINKDEF_HEADER}
  DEPENDENCIES ${SCHEMA_DICT_DEPENDENCIES}
)

ROOT_GENERATE_DICTIONARY(G__DuneClassSchema
  ${CLASS_SCHEMA_HEADER}
  LINKDEF ${CLASS_LINKDEF_HEADER}
)

add_library(dune_schema_dict_obj OBJECT
  G__DuneSchema.cxx
)

add_library(dune_class_schema_dict_obj OBJECT
  G__DuneClassSchema.cxx
)

target_include_directories(dune_schema_dict_obj PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(dune_class_schema_dict_obj PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dune_schema_dict_obj PUBLIC
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

target_link_libraries(dune_class_schema_dict_obj PUBLIC
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

add_executable(write_example_rntuple
  src/write_example_rntuple.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(write_example_rntuple PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(write_example_rntuple PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

add_executable(write_example_class_rntuple
  src/write_example_class_rntuple.cxx
  $<TARGET_OBJECTS:dune_class_schema_dict_obj>
)

target_include_directories(write_example_class_rntuple PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(write_example_class_rntuple PRIVATE
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

add_executable(summarize_class_vectors
  src/summarize_class_vectors.cxx
  $<TARGET_OBJECTS:dune_class_schema_dict_obj>
)

target_include_directories(summarize_class_vectors PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(summarize_class_vectors PRIVATE
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

add_executable(plot_track_lengths_class
  src/plot_track_lengths_class.cxx
  $<TARGET_OBJECTS:dune_class_schema_dict_obj>
)

target_include_directories(plot_track_lengths_class PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths_class PRIVATE
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(plot_muon_energy_resolution_class
  src/plot_muon_energy_resolution_class.cxx
  $<TARGET_OBJECTS:dune_class_schema_dict_obj>
)

target_include_directories(plot_muon_energy_resolution_class PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_muon_energy_resolution_class PRIVATE
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(calculate_selection_efficiency_class
  src/calculate_selection_efficiency_class.cxx
  $<TARGET_OBJECTS:dune_class_schema_dict_obj>
)

target_include_directories(calculate_selection_efficiency_class PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(calculate_selection_efficiency_class PRIVATE
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)

add_executable(plot_track_lengths
  src/plot_track_lengths.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_track_lengths PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(plot_track_lengths_rdf
  src/plot_track_lengths_rdf.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_track_lengths_rdf PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths_rdf PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(plot_track_lengths_simple
  src/plot_track_lengths_simple.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_track_lengths_simple PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths_simple PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(plot_track_lengths_simple_rdf
  src/plot_track_lengths_simple_rdf.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_track_lengths_simple_rdf PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths_simple_rdf PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
)

add_executable(plot_track_lengths_simple_better
  src/plot_track_lengths_simple_better.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_track_lengths_simple_better PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_track_lengths_simple_better PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::ROOTDataFrame
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(plot_muon_energy_resolution
  src/plot_muon_energy_resolution.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(plot_muon_energy_resolution PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plot_muon_energy_resolution PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
  ROOT::Hist
  ROOT::Gpad
  ROOT::Graf
)

add_executable(calculate_selection_efficiency
  src/calculate_selection_efficiency.cxx
  $<TARGET_OBJECTS:dune_schema_dict_obj>
)

target_include_directories(calculate_selection_efficiency PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(calculate_selection_efficiency PRIVATE
  ROOT::Core
  ROOT::GenVector
  ROOT::ROOTVecOps
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)
