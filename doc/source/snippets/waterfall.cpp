#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/gradient.hpp>
#include <mln/morpho/waterfall.hpp>

#include <mln/core/se/mask2d.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>

#include <iostream>
#include <span>
#include <string>

#include "lut.hpp"

static const mln::point2d lena_seeds[] = {
  mln::point2d{478, 19},
  mln::point2d{14, 24},
  mln::point2d{210, 26},
  mln::point2d{434, 36},
  mln::point2d{104, 37},
  mln::point2d{306, 40},
  mln::point2d{401, 41},
  mln::point2d{486, 50},
  mln::point2d{349, 53},
  mln::point2d{46, 69},
  mln::point2d{13, 96},
  mln::point2d{351, 99},
  mln::point2d{477, 115},
  mln::point2d{12, 155},
  mln::point2d{489, 165},
  mln::point2d{419, 171},
  mln::point2d{93, 209},
  mln::point2d{13, 217},
  mln::point2d{320, 225},
  mln::point2d{379, 239},
  mln::point2d{268, 265},
  mln::point2d{333, 269},
  mln::point2d{45, 277},
  mln::point2d{402, 286},
  mln::point2d{356, 300},
  mln::point2d{438, 301},
  mln::point2d{330, 306},
  mln::point2d{260, 309},
  mln::point2d{13, 314},
  mln::point2d{115, 323},
  mln::point2d{490, 341},
  mln::point2d{196, 347},
  mln::point2d{294, 356},
  mln::point2d{350, 379},
  mln::point2d{262, 410},
  mln::point2d{394, 420},
  mln::point2d{195, 426},
  mln::point2d{324, 426},
  mln::point2d{447, 431},
  mln::point2d{17, 441},
  mln::point2d{484, 460},
  mln::point2d{108, 468},
  mln::point2d{257, 477},
  mln::point2d{46, 478},
  mln::point2d{5, 481},
  mln::point2d{393, 490},
  mln::point2d{356, 492},
};

mln::image2d<std::uint8_t> make_seeds_from_points(const mln::box2d& domain, std::span<const mln::point2d> points)
{
  mln::image2d<std::uint8_t> res(domain, mln::image_build_params{-1, std::make_any<std::uint8_t>(0)});

  for (auto p : points)
    res(p) = 1;

  return res;
}

mln::image2d<int> ucm(const mln::morpho::component_tree<int>& t, mln::image2d<int> nm)
{
  using namespace mln::view::ops;
  mln::image2d<int> res        = mln::imconcretize(nm).set_init_value(0);
  auto              mask       = mln::se::mask2d{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};
  auto              err        = mln::morpho::erosion(nm, mask);
  auto              dil        = mln::morpho::erosion(nm, mask);
  auto              boundaries = (err != dil);
  const auto        depth      = t.compute_depth();
  const auto        lca        = [&depth, &t](int a, int b) {
    while (depth[a] > depth[b])
      b = t.parent[b];
    while (depth[b] > depth[a])
      a = t.parent[a];

    while (a != b)
    {
      a = t.parent[a];
      b = t.parent[b];
    }
    return a;
  };
  mln_foreach (auto p, nm.domain())
  {
    for (auto q : mln::c8(p))
    {
      if (nm.domain().has(q) && boundaries(p) && !boundaries(q) && nm(p) != nm(q))
      {
        std::cout << "nm(p) = " << nm(p) << " nm(q) = " << nm(q) << " ";
        res(p) = std::max(res(p), t.values[lca(nm(p), nm(q))]);
        std::cout << "res(p) = " << res(p);
      }
    }
  }
  return res;
}

int main(int argc, char* argv[])
{
  using namespace mln::view::ops;
  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " threshold input output_watershed output_segmentation [seeds]\n";
    return 1;
  }
  int         threshold = std::atoi(argv[1]);
  std::string input_filename(argv[2]);
  std::string output_watershed(argv[3]);
  std::string output_segmentation_filename(argv[4]);
  bool        from_markers = argc >= 6;

  // Reading the input image
  mln::image2d<std::uint8_t> input;
  mln::io::imread(input_filename, input);

  mln::image2d<int> nodemap;
  mln::morpho::component_tree<int> t;

  // Computing the gradient
  //auto grad = mln::morpho::gradient(input, mln::se::disc(3));


  if (from_markers)
  {
    mln::image2d<std::uint8_t> markers = make_seeds_from_points(input.domain(), std::span{lena_seeds});
    const auto to_save = mln::morpho::dilation(markers, mln::se::disc(5));
    mln::io::imsave(mln::view::ifelse(to_save > 0, mln::rgb8{255, 0, 0}, mln::rgb8{0, 0, 0}), argv[5]);

    std::tie(t, nodemap) = mln::morpho::waterfall_from_markers(input, markers, mln::c8);
  }
  else
  {
    // Computing the waterfall
    std::tie(t, nodemap) = mln::morpho::waterfall(input, mln::c8);
  }

  // Save the watershed
  mln::io::imsave(mln::view::transform(nodemap, [](auto x) { return regions_lut(x); }), output_watershed);

  auto cut = t.horizontal_cut(threshold, nodemap);
  mln::io::imsave(mln::view::transform(cut, [](auto x) { return regions_lut(x); }), output_segmentation_filename);

  auto saliency = ucm(t, nodemap);
  mln::io::imsave(mln::view::cast<std::uint8_t>(saliency), "sal.pgm");

  return 0;
}