#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/max.hpp>
#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/extension/padding.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/tos.hpp>

#include <mln/morpho/private/satmaxtree.hpp>
#include <mln/morpho/private/trees_fusion.hpp>

#include <iostream>
#include <vector>

#include "lut.hpp"

namespace
{
  mln::rgb8 get_median_on_border(mln::image2d<mln::rgb8> ima)
  {
    std::vector<mln::rgb8> border;
    border.reserve(2 * ima.width() + 2 * ima.height() - 4);

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
      int i = 0;
      while (i < 3 && a[i] == b[i])
        i++;
      return i < 3 && a[i] < b[i];
    });
    return border[border.size() / 2];
  }

  /// \brief Reduce the size of the nodemap resulting from the MToS computation
  ///        to fit the original image one
  mln::image2d<int> reduce_nodemap(mln::image2d<int> n)
  {
    const mln::point2d pmin(n.domain().tl());
    const mln::point2d pmax(n.domain().br());
    mln::image2d<int>  res(mln::box2d{{(pmin[0] - 3) / 4, (pmin[1] - 3) / 4}, {(pmax[0] + 3) / 4, (pmax[1] + 3) / 4}});

    for (int y = 0; y < res.height(); y++)
    {
      int* in  = n.buffer() + y * 4 * n.stride(1);
      int* out = res.buffer() + y * res.stride();
      for (int x = 0; x < res.width(); x++)
      {
        *out = *in;
        in += 4;
        out++;
      }
    }

    return res;
  }
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cerr << "Invalid number of arguments\nUsage: " << argv[0]
              << " input_filename depth_map_filename rec_filename\n";
    return 1;
  }

  mln::image2d<mln::rgb8> ima;
  mln::io::imread(argv[1], ima);

  const auto med = get_median_on_border(ima);
  ima.inflate_domain(1);
  constexpr int borders[2][2] = {{1, 1}, {1, 1}};
  mln::pad(ima, mln::PAD_CONSTANT, borders, med);

  mln::morpho::component_tree<> trees[3];
  mln::image2d<int>             nodemaps[3];
  std::vector<int>              depths[3];

  for (int c = 0; c < 3; c++)
  {
    std::tie(trees[c], nodemaps[c]) = mln::morpho::tos(mln::view::channel(ima, c), {-1, -1});
    depths[c]                       = trees[c].compute_depth();
  }

  const auto [gos, tree_to_graph] = mln::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);
  auto depth_map                  = mln::morpho::details::compute_depth_map(gos, tree_to_graph, nodemaps);
  {
    std::uint16_t max_depth = mln::accumulate(depth_map, mln::accu::accumulators::max<std::uint16_t>());
    auto          normalized_depth =
        mln::view::transform(depth_map, [&max_depth](std::uint16_t a) -> float { return (float)a / (float)max_depth; });
    auto heat_depth = mln::view::transform(normalized_depth, heat_lut);
    mln::io::imsave(heat_depth, argv[2]);
  }

  auto [t, nm] = mln::morpho::details::satmaxtree(depth_map, {-1, -1});
  nm           = reduce_nodemap(nm);
  ima.inflate_domain(-1);
  nm.inflate_domain(-1);

  auto area = t.compute_attribute_on_points(nm, mln::accu::accumulators::count<int>());
  t.filter(mln::morpho::CT_FILTER_DIRECT, nm, [&area](int n) { return area[n] >= 100; });
  auto mean = t.compute_attribute_on_values(nm, ima, mln::accu::features::mean<>(), false);
  auto rec  = t.reconstruct_from(nm, ranges::make_span(mean.data(), mean.size()));
  mln::io::imsave(mln::view::cast<mln::rgb8>(rec), argv[3]);

  return 0;
}