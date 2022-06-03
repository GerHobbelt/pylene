#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/transforms/hough_lines.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>

#include <fmt/core.h>
#include <cairo/cairo.h>
#include <cmath>
#include <iostream>


struct argb8 {
  uint8_t a, r, g, b;
};

void render_lines(mln::image2d<argb8> input, std::array<double, 4> lines[], std::size_t n, const char* filepath)
{

  auto surface = cairo_image_surface_create_for_data ((uint8_t*)input.buffer(), CAIRO_FORMAT_ARGB32, input.width(), input.height(), input.byte_stride());

  if (auto st = cairo_surface_status (surface); st != CAIRO_STATUS_SUCCESS) {
    fmt::print("Error when creating surface.\n");
    if (st == CAIRO_STATUS_INVALID_STRIDE)
      fmt::print("Invalid stride.\n");
    auto s = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, input.width());
    fmt::print("{} vs {} w={}\n", input.byte_stride(), s, input.width());
  }

  auto ctx = cairo_create (surface);
  

  cairo_set_line_width (ctx, 5);
  cairo_set_source_rgb (ctx, 1.0, 0, 0);
  for (std::size_t i = 0; i < n; ++i)
  {
    fmt::print("(x={},y={}) -> (x={},y={})\n", lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
    cairo_move_to(ctx, lines[i][0], lines[i][1]);
    cairo_line_to(ctx, lines[i][2], lines[i][3]);
  }
  cairo_stroke (ctx);
  cairo_destroy(ctx);
  cairo_surface_write_to_png(surface, filepath); 
  cairo_surface_destroy(surface);
}

void render_lines(mln::image2d<argb8> input, const std::vector<mln::transforms::HoughLine>& lines, const char* out)
{
  using T = std::array<double, 4>;
  auto coords = std::vector<T>(lines.size());

  std::size_t n = lines.size();
  for (std::size_t i = 0; i < n; ++i)
  {
    auto a = lines[i].angle; // * M_PI / 180;
    double ca = std::cos(a);
    double sa = std::sin(a);
    double r = std::hypot(input.width(), input.height());
    double cx = ca * lines[i].radius;
    double cy = sa * lines[i].radius;

    coords.push_back(T{cy + r * ca, cx - r * sa, cy - r * ca, cx + r * sa });
  }

  render_lines(input, coords.data(), coords.size(), out);
}


int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input.pbm out.png\n";
    std::exit(1);
  }

  mln::image2d<bool> input;
  mln::io::imread(argv[1], input);


  { // #BEGIN
    std::vector<float> angles =
    {-0.01745329, -0.01570796, -0.01396263, -0.0122173 , -0.01047198, //
     -0.00872665, -0.00698132, -0.00523599, -0.00349066, -0.00174533, //
      0.        ,  0.00174533,  0.00349066,  0.00523599,  0.00698132, //
      0.00872665,  0.01047198,  0.0122173 ,  0.01396263,  0.01570796, //
      0.01745329};



    auto votes = mln::transforms::hough_lines(input, angles);
    auto peaks = mln::transforms::hough_lines_detect_peaks(votes, angles, .5f, 10, 15);

    for (auto p : peaks)
      fmt::print("angle={}; radius={}; count={}\n", p.angle * 180 / M_PI, p.radius, p.count);

    mln::image2d<argb8> out(input.width(), input.height(), mln::image_build_params { .border = 0, .init_value = {} });
    mln::transform(input, out, [](bool x) { return !x ? argb8{255,255,255,255} : argb8{0,0,0,255}; });
    render_lines(out, peaks, argv[2]);
  } // #END


}
