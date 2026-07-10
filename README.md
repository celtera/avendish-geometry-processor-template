# Avendish geometry / mesh template

[![Build](https://github.com/celtera/avendish-geometry-processor-template/actions/workflows/builds.yaml/badge.svg)](https://github.com/celtera/avendish-geometry-processor-template/actions/workflows/builds.yaml)

This provides a basic, canonical template for making **geometry / mesh** objects with
[Avendish](https://github.com/celtera/avendish): a single C++ object that generates a
mesh and is compiled, without rewrite, to several host plug-in formats.

It is the geometry counterpart of the
[audio processor template](https://github.com/celtera/avendish-audio-processor-template):
where that one ticks per audio buffer and processes samples, this one ticks per frame
and produces a mesh (vertices, normals, colors, indices) on its output.

## What gets built

The `avnd_addon_object(... CATEGORY geometry)` call in `CMakeLists.txt` instantiates the geometry
back-ends. From `src/Model.hpp` you get:

| Back-end | Object kind | SDK required |
|---|---|---|
| **TouchDesigner SOP** | Surface Operator | TouchDesigner Custom Operator SDK |
| **TouchDesigner POP** | Particle Operator | TouchDesigner Custom Operator SDK |
| **Godot** | `GDExtension` node producing an `ArrayMesh` | none — `godot-cpp` is fetched automatically |
| **ossia score** | geometry process | libossia |

Back-ends whose SDK is not provided are silently skipped, so you can build just the
ones you need.

> **Note on Max/MSP:** Max is intentionally *not* a geometry target. Max's Avendish
> binding (Jitter) only handles matrix / texture outputs and rejects geometry output
> ports at compile time, so a geometry object cannot be emitted for Max. Use the
> [audio/message template](https://github.com/celtera/avendish-audio-processor-template)
> for Max objects.

## The object

`src/Model.hpp` / `src/Model.cpp` implement `MyGeometry`, a generator that emits a
subdivided plane mesh displaced by a sine wave, with controllable size, subdivision
count, displacement height and color. It writes positions, normals, per-vertex colors
and a triangle index buffer into a
[`halp::position_normals_color_index_geometry`](https://github.com/celtera/avendish/blob/main/include/halp/geometry.hpp)
output.

Start from there: change the ports in the `inputs` struct, and rewrite `operator()` to
fill the mesh buffers however you like. See
[`examples/Tutorial/CubeGenerator.hpp`](https://github.com/celtera/avendish/blob/main/examples/Tutorial/CubeGenerator.hpp)
in Avendish for the canonical reference, and `halp/geometry.hpp` for the other ready-made
mesh layouts (position-only point clouds, packed/interleaved attributes, per-attribute
buffers, fully dynamic run-time geometry, …).

## Building

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

With no extra options this builds the **Godot** back-end (it fetches `godot-cpp`
automatically). To enable the other back-ends, pass the relevant SDK paths below.

To see a complete build procedure, refer to the
[Github actions workflows](.github/workflows/), which compile the project on clean
virtual machines.

### Godot

`godot-cpp` is fetched via CMake `FetchContent` by default. To build against an existing
checkout instead, pass:

```cmake
-DGODOT_CPP_PATH=path/to/godot-cpp
```

The build produces `build/godot/my_geometry_geo.{so,dll,dylib}` and a matching
`.gdextension` file you can drop into a Godot project.

### TouchDesigner (SOP + POP)

Point CMake at a TouchDesigner Custom Operator SDK whose `include/` directory contains
the operator base headers. Avendish reads `${TOUCHDESIGNER_SDK_PATH}/include/`:

```cmake
-DTOUCHDESIGNER_SDK_PATH=path/to/CustomOperatorSamples
```

This produces a TouchDesigner **SOP** (`build/td/...SOP...`) and a TouchDesigner
**POP** (`build/td/...POP...`).

- The **SOP** back-end only needs `CPlusPlus_Common.h` + `SOP_CPlusPlusBase.h`, present
  in the public [`TouchDesigner/CustomOperatorSamples`](https://github.com/TouchDesigner/CustomOperatorSamples).
- The **POP** back-end is experimental and additionally needs `POP_CPlusPlusBase.h` plus
  the POP-enabled `CPlusPlus_Common.h`, which Derivative ships only inside the
  experimental TouchDesigner installation (`Samples/CPlusPlus`). The
  [`jcelerier/CustomOperatorSamples`](https://github.com/jcelerier/CustomOperatorSamples)
  fork bundles these under `include/`, so pointing `TOUCHDESIGNER_SDK_PATH` at a checkout
  of that fork builds both SOP and POP. This is what the CI does.

Note: TouchDesigner uses the MSVC ABI on Windows — build with clang-cl or MSVC, not MinGW
(the CI builds the TouchDesigner back-ends on Linux and macOS with Clang).

### ossia score

Install [libossia](https://github.com/ossia/libossia) (or build score and point CMake at
its 3rdparty tree) so that `find_package(ossia)` succeeds; the geometry process then
becomes available inside ossia score.

## TODO

* Support importing the template with a package manager
* Max/MSP (Jitter) mesh output
