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
#include <string>

#include "lut.hpp"

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
    std::cerr << "Usage: " << argv[0] << " threshold input output_watershed output_segmentation\n";
    return 1;
  }
  int         threshold = std::atoi(argv[1]);
  std::string input_filename(argv[2]);
  std::string output_watershed(argv[3]);
  std::string output_segmentation_filename(argv[4]);

  // Reading the input image
  mln::image2d<std::uint8_t> input;
  mln::io::imread(input_filename, input);

  // Computing the gradient
  auto grad = mln::morpho::gradient(input, mln::se::disc(3));

  // Computing the waterfall
  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

  // Save the watershed
  mln::io::imsave(mln::view::transform(nodemap, [](auto x) { return regions_lut(x); }), output_watershed);

  auto cut = t.horizontal_cut(threshold, nodemap);
  mln::io::imsave(mln::view::transform(cut, [](auto x) { return regions_lut(x); }), output_segmentation_filename);

  auto saliency = ucm(t, nodemap);
  mln::io::imsave(mln::view::cast<std::uint8_t>(saliency), "sal.pgm");

  return 0;
}