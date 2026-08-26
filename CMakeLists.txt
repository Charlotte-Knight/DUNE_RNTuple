cmake_minimum_required(VERSION 3.16)
project(DUNE_RNTuple_Example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

set(SCHEMA_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/Schema.h)
set(LINKDEF_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h)

ROOT_GENERATE_DICTIONARY(G__DuneSchema
  ${SCHEMA_HEADER}
  LINKDEF ${LINKDEF_HEADER}
)

add_library(dune_schema_dict_obj OBJECT
  G__DuneSchema.cxx
)

target_include_directories(dune_schema_dict_obj PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dune_schema_dict_obj PUBLIC
  ROOT::Core
  ROOT::GenVector
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
  ROOT::RIO
  ROOT::Tree
  ROOT::ROOTNTuple
)
