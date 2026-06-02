#include "Model.hpp"

#include <cmath>
#include <cstring>
#include <numbers>

void MyGeometry::operator()()
{
  const float size = inputs.size;
  const int n = inputs.subdivisions; // cells per side
  const float h = inputs.height;
  const std::array<float, 4> col{
      inputs.color.value.r, inputs.color.value.g, inputs.color.value.b,
      inputs.color.value.a};

  const int side = n + 1;            // vertices per side
  const int vertex_count = side * side;

  positions.clear();
  normals.clear();
  colors.clear();
  indices.clear();
  positions.reserve(vertex_count);
  normals.reserve(vertex_count);
  colors.reserve(vertex_count);
  indices.reserve(n * n * 6);

  // Sinusoidal displacement Y(u, v), with u, v in [0, 1].
  constexpr float tau = 2.f * std::numbers::pi_v<float>;
  const auto Y = [=](float u, float v) {
    return h * std::sin(u * tau) * std::cos(v * tau);
  };

  // Generate the grid of vertices on the XZ plane, displaced along Y.
  for(int j = 0; j < side; ++j)
  {
    const float v = (side > 1) ? float(j) / float(n) : 0.f;
    for(int i = 0; i < side; ++i)
    {
      const float u = (side > 1) ? float(i) / float(n) : 0.f;

      const float x = (u - 0.5f) * size;
      const float z = (v - 0.5f) * size;
      const float y = Y(u, v);
      positions.push_back({x, y, z});

      // Analytic normal from the partial derivatives of the surface.
      // dP/du = (size, dY/du, 0), dP/dv = (0, dY/dv, size).
      // normal = normalize(cross(dP/dv, dP/du)) -> points towards +Y.
      const float dYdu = h * tau * std::cos(u * tau) * std::cos(v * tau);
      const float dYdv = -h * tau * std::sin(u * tau) * std::sin(v * tau);
      float nx = -dYdu * size;
      float ny = size * size;
      float nz = -dYdv * size;
      const float len = std::sqrt(nx * nx + ny * ny + nz * nz);
      if(len > 0.f)
      {
        nx /= len;
        ny /= len;
        nz /= len;
      }
      normals.push_back({nx, ny, nz});

      colors.push_back(col);
    }
  }

  // Two counter-clockwise triangles per grid cell (front face towards +Y).
  for(int j = 0; j < n; ++j)
  {
    for(int i = 0; i < n; ++i)
    {
      const uint32_t v00 = uint32_t(j * side + i);
      const uint32_t v10 = v00 + 1;
      const uint32_t v01 = uint32_t((j + 1) * side + i);
      const uint32_t v11 = v01 + 1;

      indices.insert(indices.end(), {v00, v11, v10});
      indices.insert(indices.end(), {v00, v01, v11});
    }
  }

  // Point the geometry buffers at our persistent storage.
  auto& mesh = outputs.geometry.mesh;
  mesh.buffers.position_buffer.elements = reinterpret_cast<float*>(positions.data());
  mesh.buffers.position_buffer.element_count = positions.size();
  mesh.buffers.position_buffer.dirty = true;
  mesh.buffers.normal_buffer.elements = reinterpret_cast<float*>(normals.data());
  mesh.buffers.normal_buffer.element_count = normals.size();
  mesh.buffers.normal_buffer.dirty = true;
  mesh.buffers.color_buffer.elements = reinterpret_cast<float*>(colors.data());
  mesh.buffers.color_buffer.element_count = colors.size();
  mesh.buffers.color_buffer.dirty = true;
  mesh.buffers.index_buffer.elements = indices.data();
  mesh.buffers.index_buffer.element_count = indices.size();
  mesh.buffers.index_buffer.dirty = true;

  mesh.vertices = vertex_count;

  // Identity transform.
  std::memset(outputs.geometry.transform, 0, sizeof(outputs.geometry.transform));
  outputs.geometry.transform[0] = 1.f;
  outputs.geometry.transform[5] = 1.f;
  outputs.geometry.transform[10] = 1.f;
  outputs.geometry.transform[15] = 1.f;

  outputs.geometry.dirty_mesh = true;
  outputs.geometry.dirty_transform = true;
}
