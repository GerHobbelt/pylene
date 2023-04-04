#include <scribo/segdet.hpp>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <cairo/cairo.h>
#include <fmt/core.h>
#include <iostream>

#include "lut.hpp"

struct argb8
{
  uint8_t a, r, g, b;
};

void render_image_vector(mln::image2d<argb8> input, std::vector<scribo::VSegment> segments, const char* filepath)
{
  auto surface = cairo_image_surface_create_for_data((uint8_t*)input.buffer(), CAIRO_FORMAT_ARGB32, input.width(),
                                                     input.height(), input.byte_stride());

  if (auto st = cairo_surface_status(surface); st != CAIRO_STATUS_SUCCESS)
  {
    fmt::print("Error when creating surface.\n");
    if (st == CAIRO_STATUS_INVALID_STRIDE)
      fmt::print("Invalid stride.\n");
    auto s = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, input.width());
    fmt::print("{} vs {} w={}\n", input.byte_stride(), s, input.width());
  }

  auto ctx = cairo_create(surface);

  cairo_set_line_width(ctx, 5);
  for (auto& segment : segments)
  {
    fmt::print("(x={},y={}) -> (x={},y={})\n", segment.x0, segment.y0, segment.x1, segment.y1);

    auto color = regions_lut(segment.label);
    cairo_set_source_rgb(ctx, color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
    cairo_move_to(ctx, segment.x0, segment.y0);
    cairo_line_to(ctx, segment.x1, segment.y1);
    cairo_stroke(ctx);
  }

  cairo_stroke(ctx);
  cairo_destroy(ctx);
  cairo_surface_write_to_png(surface, filepath);
  cairo_surface_destroy(surface);
}

int main(int argc, char** argv)
{
  if (argc != 4)
  {
    std::cerr << "Usage: " << argv[0] << " input.pgm output.png\n";
    std::exit(1);
  }

  mln::image2d<std::uint8_t> input;
  mln::io::imread(argv[1], input);

  const char* output_filepath_vector = argv[2];
  const char* output_filepath_pixel  = argv[3];

  {
    // #BEGIN 1
    int                  min_len = 20;
    scribo::SegDetParams params  = {.traversal_mode         = scribo::e_segdet_process_traversal_mode::HORIZONTAL,
                                    .discontinuity_relative = 30,
                                    .max_thickness          = 5};
    auto [labelled_image, superposition_vector, vsegment_vector] = scribo::detect_line_full(input, min_len, params);
    // #END 1

    // #BEGIN 2
    mln::image2d<argb8> out_vector(input.width(), input.height(),
                                   mln::image_build_params{.border = 0, .init_value = {}});
    mln::transform(input, out_vector, [](int x) { return x != 0 ? argb8{255, 255, 255, 255} : argb8{0, 0, 0, 255}; });
    render_image_vector(out_vector, vsegment_vector, output_filepath_vector);
    // #END 2

    // #BEGIN 3
    mln_foreach (auto pt, labelled_image.domain())
      labelled_image(pt) = labelled_image(pt) == 0 ? (input(pt) != 0 ? 1 : 0) : labelled_image(pt);
    mln::io::imsave(mln::view::transform(labelled_image, [](auto x) { return regions_lut(x); }), output_filepath_pixel);
    // #END 3

    // #BEGIN 4
    for (auto superposition : superposition_vector)
      fmt::print("label={}; x={}; y={}\n", superposition.label, superposition.x, superposition.y);
    // #END 4
  }
}