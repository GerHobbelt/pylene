#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/tos.hpp>

#include <mln/morpho/private/trees_fusion.hpp>

#include <iostream>
#include <vector>

#include "lut.hpp"

namespace
{
  mln::image2d<mln::rgb8> add_border(mln::image2d<mln::rgb8> ima)
  {
    mln::image2d<mln::rgb8> res(ima.width() + 2, ima.height() + 2);

    std::vector<mln::rgb8> border;
    border.reserve(2 * ima.width() + 2 * ima.height() - 4);

    mln_foreach (auto p, ima.domain())
      res(mln::point2d{1, 1} + p) = ima(p);

    for (int i = 0; i < ima.width(); i++)
    {
      border.push_back(ima(mln::point2d{i, 0}));
      border.push_back(ima(mln::point2d{i, ima.height() - 1}));
    }

    for (int i = 1; i < ima.height() - 1; i++)
    {
      border.push_back(ima(mln::point2d{0, i}));
      border.push_back(ima(mln::point2d{ima.width() - 1, i}));
    }

    std::sort(border.begin(), border.end(), [](const mln::rgb8& a, const mln::rgb8& b) {
      for (int i = 0; i < 3; i++)
      {
        if (a[i] < b[i])
          return true;
        else if (b[i] < a[i])
          return false;
      }
      return false;
    });
    mln::rgb8 med = border[border.size() / 2];

    for (int i = 0; i < res.width(); i++)
    {
      res(mln::point2d{i, 0})                = med;
      res(mln::point2d{i, res.height() - 1}) = med;
    }
    for (int i = 1; i < res.height() - 1; i++)
    {
      res(mln::point2d{0, i})               = med;
      res(mln::point2d{res.width() - 1, i}) = med;
    }

    return res;
  }

  std::uint16_t max(mln::image2d<std::uint16_t> ima)
  {
    std::uint16_t res = 0;
    mln_foreach (auto p, ima.domain())
      res = std::max(res, ima(p));
    return res;
  }

  
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Invalid number of arguments\nUsage: " << argv[0] << " input_filename depth_map_filename\n";
    return 1;
  }

  mln::image2d<mln::rgb8> ima;
  mln::io::imread(argv[1], ima);
  ima = add_border(ima);

  mln::morpho::component_tree<> trees[3];
  mln::image2d<int>             nodemaps[3];
  std::vector<int>              depths[3];

  for (int c = 0; c < 3; c++)
  {
    std::tie(trees[c], nodemaps[c]) = mln::morpho::tos(mln::view::channel(ima, c), {0, 0});
    depths[c]                       = trees[c].compute_depth();
  }


  const auto [gos, tree_to_graph] = mln::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);
  auto depth_map                  = mln::morpho::details::compute_depth_map(gos, tree_to_graph, nodemaps);
  std::uint16_t max_depth = max(depth_map);
  auto normalized_depth = mln::view::transform(depth_map, [&max_depth](std::uint16_t a) -> float { return (float)a / (float)max_depth;});
  auto heat_depth = mln::view::transform(normalized_depth, heat_lut);

  mln::io::imsave(heat_depth, argv[2]);

  return 0;
}