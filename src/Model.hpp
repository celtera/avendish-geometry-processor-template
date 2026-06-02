#pragma once
#include <halp/controls.hpp>
#include <halp/geometry.hpp>
#include <halp/meta.hpp>

#include <array>
#include <cstdint>
#include <vector>

// A minimal vec3, used to fill the geometry buffers.
struct vec3
{
  float x, y, z;
};

// MyGeometry is a geometry / mesh "generator": it has no geometry input, and
// produces a single mesh on its output every time it is ticked.
//
// The very same object compiles to:
//   - a TouchDesigner SOP (Surface Operator)
//   - a TouchDesigner POP (Particle Operator)
//   - a Godot GDExtension node producing an ArrayMesh
//   - an ossia score process
//
// See https://github.com/celtera/avendish/blob/main/examples/Tutorial/CubeGenerator.hpp
// for the canonical example this template is derived from.
class MyGeometry
{
public:
  halp_meta(name, "My Geometry")
  halp_meta(c_name, "my_geometry")
  halp_meta(category, "Generator")
  halp_meta(author, "Avendish")
  halp_meta(description, "Generate a subdivided, displaced plane mesh")

  // CHANGE THIS !!
  // - On linux: uuidgen | xargs printf | xclip -selection clipboard
  //   will copy one on the clipboard
  // - uuidgen exists on Mac and Windows too
  halp_meta(uuid, "027f6de2-a635-46a5-96ca-ea90c8d3c3c7")

  // Named so that the UI (UI.hpp) can reference the ports via halp::item<&ins::...>.
  struct ins
  {
    // Width / depth of the plane.
    halp::hslider_f32<"Size", halp::range{.min = 0.1f, .max = 10.f, .init = 1.f}> size;

    // Number of subdivisions per side: (N+1) x (N+1) vertices, 2*N*N triangles.
    halp::spinbox_i32<"Subdivisions", halp::range{.min = 1, .max = 256, .init = 16}>
        subdivisions;

    // Amplitude of the sinusoidal displacement applied along the Y axis.
    halp::hslider_f32<"Height", halp::range{.min = 0.f, .max = 2.f, .init = 0.3f}> height;

    // Vertex color, applied uniformly across the mesh.
    halp::color_chooser<"Color"> color;
  } inputs;

  struct
  {
    // The geometry output. Avendish recognizes the mesh by its inner
    // halp::*_geometry member, and exposes the transform + dirty flags so that
    // hosts can avoid re-uploading unchanged data.
    struct
    {
      halp_meta(name, "Geometry")
      halp::position_normals_color_index_geometry mesh;
      float transform[16]{};
      bool dirty_mesh = false;
      bool dirty_transform = false;
    } geometry;
  } outputs;

  // Recompute the mesh and push it to the geometry output.
  void operator()();

  // Defined in UI.hpp
  struct ui;

private:
  // Persistent storage backing the geometry buffers. The geometry output only
  // stores pointers into these vectors, so they must outlive each operator()
  // call until the host has consumed the mesh (i.e. at least until the next tick).
  std::vector<vec3> positions;
  std::vector<vec3> normals;
  std::vector<std::array<float, 4>> colors;
  std::vector<uint32_t> indices;
};
