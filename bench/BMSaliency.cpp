#include <mln/core/colors.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/component_tree.hpp>
#include <mln/morpho/lca.hpp>

#include <benchmark/benchmark.h>

/**
 * Saliency function to be used with several LCA
 */
template <typename LCA, typename V>
auto saliency(const mln::morpho::component_tree<V>& t, mln::image2d<int> node_map)
{
  LCA lca(t);

  const auto           kwidth  = node_map.width() * 2 - 1;
  const auto           kheight = node_map.height() * 2 - 1;
  mln::image2d<double> res(kwidth, kheight);
  mln::fill(res, 0);

  mln_foreach (auto p, node_map.domain())
  {
    for (auto q : mln::c4.after(p))
    {
      if (node_map.domain().has(q))
      {
        const auto dir                              = q - p;
        res(mln::point2d{2 * p[0], 2 * p[1]} + dir) = t.values[lca(node_map(p), node_map(q))];
      }
    }
  }
  mln_foreach (auto p, res.domain())
  {
    if (p[0] % 2 == 1 && p[1] % 2 == 1)
    {
      for (auto q : mln::c4(p))
      {
        if (res.domain().has(q) && res(p) < res(q))
          res(p) = res(q);
      }
    }
  }
  return res;
}

class BMSaliency : public benchmark::Fixture
{
public:
  BMSaliency()
  {
    mln::image2d<mln::rgb8> ima;
    mln::io::imread("Aerial_view_of_Olbia.jpg", ima);
    std::tie(m_tree, m_nodemap) = mln::morpho::alphatree(ima, mln::c4, [](auto a, auto b) -> double { return mln::functional::l2dist(a, b); });
  }

  template <typename LCA>
  void run(benchmark::State& st)
  {
    for (auto _ : st)
      saliency<LCA>(m_tree, m_nodemap);
  }

protected:
  mln::morpho::component_tree<double> m_tree;
  mln::image2d<int>                      m_nodemap;
};

BENCHMARK_F(BMSaliency, CurrentSaliency)(benchmark::State& st)
{
  for (auto _ : st)
    this->m_tree.saliency(m_nodemap, ::ranges::make_span(m_tree.values.data(), m_tree.values.size()));
}

BENCHMARK_F(BMSaliency, SaliencySparseTable)(benchmark::State& st)
{
  this->run<mln::morpho::lca>(st);
}

BENCHMARK_MAIN();