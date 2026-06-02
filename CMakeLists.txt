cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(MyGeometry CXX)

include(dependencies.cmake)

add_library(MyGeometry STATIC
  src/Model.cpp
  src/Model.hpp
  src/UI.hpp
  src/Processor.hpp
)

# avnd_make_geometry instantiates the geometry / mesh back-ends:
#  - ossia (for use inside ossia score)
#  - TouchDesigner SOP (Surface Operator)
#  - TouchDesigner POP (Particle Operator)
#  - Godot GDExtension (a Node3D producing an ArrayMesh)
#
# The relevant SDK paths must be passed to CMake for the optional back-ends to
# be built, see the README. Back-ends whose SDK is not found are silently skipped.
avnd_make_geometry(
  TARGET MyGeometry
  MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/Processor.hpp"
  MAIN_CLASS MyGeometry
  C_NAME my_geometry
)

# Workaround: when godot-cpp is pulled in via FetchContent, its only targets are
# `godot-cpp` (+ alias `godot::cpp`) — there is no `godot-cpp::godot-cpp`. Avendish's
# avnd_make_godot doesn't always resolve the target, so the generated headers under
# <godot-cpp-build>/gen/include never make it onto the GDExtension's include path and
# compilation fails with "godot_cpp/classes/node.hpp not found". Re-linking the target
# here (at a scope where it is visible) propagates godot-cpp's PUBLIC include dirs.
if(TARGET MyGeometry_GEOMETRY_godot AND TARGET godot-cpp)
  target_link_libraries(MyGeometry_GEOMETRY_godot PRIVATE godot-cpp)
endif()
