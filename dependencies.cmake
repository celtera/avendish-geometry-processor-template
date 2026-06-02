include(FetchContent)

FetchContent_Declare(
  avendish
  GIT_REPOSITORY "https://github.com/celtera/avendish"
  # Geometry back-ends (TouchDesigner SOP/POP, Godot GEOMETRY) require a recent
  # Avendish: the avnd_make_geometry() macro was introduced after the audio
  # template's pinned commit, so we track a newer revision here.
  GIT_TAG  dba4e800c3d27ec03f96eefdeb9d54605ce24269
  GIT_PROGRESS true
)
FetchContent_Populate(avendish)

set(CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR};${CMAKE_PREFIX_PATH}")
find_package(Avendish REQUIRED)
