#include <mln/accu/accumulators/count.hpp>
#include <mln/core/colors.hpp>
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
  struct mean_node_accu : mln::Accumulator<mean_node_accu>
  {
    using result_type = decltype(std::declval<mln::rgb8>() + std::declval<mln::rgb8>());
  public:
    void take(const mln::rgb8& v)
    {
      m_sum += v;
      m_count++;
    }

    void take(const mean_node_accu&) {}

    result_type to_result() const { return m_count > 1 ? static_cast<result_type>(m_sum / m_count) : m_sum; }

  private:
    result_type m_sum = {0, 0, 0};
    int m_count = 0;
  };

  /// \brief Add a border to the image with values set at the median value
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
      int i = 0;
      while (i < 3 && a[i] == b[i])
        i++;
      return i < 3 && a[i] < b[i];
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

  mln::image2d<int> remove_border(mln::image2d<int> n)
  {
    mln::image2d<int> res(n.width() - 2, n.height() - 2);
    mln_foreach(auto p, res.domain())
      res(p) = n(p + mln::point2d{1, 1});
    return res;
  }

  /// \brief Compute the maximum value of an image
  std::uint16_t max(mln::image2d<std::uint16_t> ima)
  {
    std::uint16_t res = 0;
    mln_foreach (auto p, ima.domain())
      res = std::max(res, ima(p));
    return res;
  }

  /// \brief Reduce the size of a nodemap by a factor 2
  mln::image2d<int> reduce_nodemap(mln::image2d<int> n)
  {
    mln::image2d<int> res((n.width() + 3) / 4, (n.height() + 3) / 4);

    mln_foreach(auto p, n.domain())
    {
      if (p[0] % 4 == 0 && p[1] % 4 == 0)
      {
        res(mln::point2d{p[0] / 4, p[1] / 4}) = n(p);
      }
    }

    return res;
  }
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cerr << "Invalid number of arguments\nUsage: " << argv[0] << " input_filename depth_map_filename rec_filename\n";
    return 1;
  }

  mln::image2d<mln::rgb8> ima;
  mln::io::imread(argv[1], ima);
  auto to_process = add_border(ima);

  mln::morpho::component_tree<> trees[3];
  mln::image2d<int>             nodemaps[3];
  std::vector<int>              depths[3];

  for (int c = 0; c < 3; c++)
  {
    std::tie(trees[c], nodemaps[c]) = mln::morpho::tos(mln::view::channel(to_process, c), {0, 0});
    depths[c]                       = trees[c].compute_depth();
  }


  const auto [gos, tree_to_graph] = mln::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);
  auto depth_map                  = mln::morpho::details::compute_depth_map(gos, tree_to_graph, nodemaps);
  {
    std::uint16_t max_depth = max(depth_map);
    auto          normalized_depth =
        mln::view::transform(depth_map, [&max_depth](std::uint16_t a) -> float { return (float)a / (float)max_depth; });
    auto heat_depth = mln::view::transform(normalized_depth, heat_lut);
    mln::io::imsave(heat_depth, argv[2]);
  }

  auto [t, nm] = mln::morpho::details::satmaxtree(depth_map, {0, 0});
  nm = reduce_nodemap(nm);
  nm = remove_border(nm);
  auto area = t.compute_attribute_on_points(nm, mln::accu::accumulators::count<int>());
  t.filter(mln::morpho::CT_FILTER_DIRECT, nm, [&area](int n) { return area[n] >= 200; });
  auto mean = t.compute_attribute_on_values(nm, ima, mean_node_accu());
  auto rec = t.reconstruct_from(nm, ranges::make_span(mean.data(), mean.size()));
  mln::io::imsave(mln::view::cast<mln::rgb8>(rec), argv[3]);

  return 0;
}