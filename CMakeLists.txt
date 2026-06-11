cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(MyGeometry CXX)

# Use the Avendish the host already provides (e.g. ossia score); otherwise fetch it.
# The same CMakeLists builds this as an ossia/score add-on or as a standalone object.
find_package(Avendish QUIET)
if(NOT Avendish_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Avendish
    GIT_REPOSITORY "https://github.com/celtera/avendish"
    GIT_TAG 6d95ff0bbb1524b5846e4222f5dc610f9e8d0b7a)
  FetchContent_Populate(Avendish)
  list(APPEND CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR}")
  find_package(Avendish REQUIRED)
endif()

avnd_addon_init(NAME MyGeometry)

# CATEGORY geometry: the geometry / mesh back-ends -- ossia, Max/MSP, TouchDesigner SOP
# and POP, and Godot (a Node3D producing an ArrayMesh). In a score build only the ossia
# process is produced. Back-ends whose SDK is absent are silently skipped.
# (For a single TD operator type, use e.g. BACKENDS godot:GEOMETRY touchdesigner:POP.)
avnd_addon_object(
  BASE MyGeometry
  C_NAME my_geometry
  CLASS MyGeometry
  CATEGORY geometry
  MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/Processor.hpp"
  SOURCES
    src/Model.cpp
    src/Model.hpp
    src/UI.hpp
    src/Processor.hpp)

avnd_addon_finalize(NAME MyGeometry UUID 95b84273-50ed-4968-b32c-5c82bf3d184c VERSION 1.0.0)
