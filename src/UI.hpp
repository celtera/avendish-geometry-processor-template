#pragma once
#include "Model.hpp"

#include <halp/layout.hpp>
#include <halp/meta.hpp>

struct MyGeometry::ui
{
  // If your compiler is recent enough:
  // using enum halp::colors;
  // using enum halp::layouts;
  halp_meta(name, "Main")
  halp_meta(layout, halp::layouts::vbox)
  halp_meta(background, halp::colors::mid)

  halp::label header{"My Geometry"};

  struct
  {
    halp_meta(layout, halp::layouts::hbox)
    halp_meta(background, halp::colors::dark)

    halp::item<&MyGeometry::ins::size> size;
    halp::item<&MyGeometry::ins::subdivisions> subdivisions;
    halp::item<&MyGeometry::ins::height> height;
    halp::item<&MyGeometry::ins::color> color;
  } widgets;
};
