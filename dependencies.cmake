include(FetchContent)

FetchContent_Declare(
  avendish
  GIT_REPOSITORY "https://github.com/celtera/avendish"
  # Geometry back-ends (TouchDesigner SOP/POP, Godot GEOMETRY, Max/MSP ob3d)
  # require a recent Avendish: avnd_make_geometry() and the Max geometry (ob3d)
  # binding were introduced after the audio template's pinned commit, so we track
  # the feature/max-obj3d revision here.
  GIT_TAG  b318737b92dcce8f0df08949759cd1bddf72c5fa
  GIT_PROGRESS true
)
FetchContent_Populate(avendish)

set(CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR};${CMAKE_PREFIX_PATH}")
find_package(Avendish REQUIRED)
