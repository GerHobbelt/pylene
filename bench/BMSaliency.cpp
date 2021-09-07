#include <mln/accu/accumulators/count.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/component_tree.hpp>
#include <mln/morpho/lca.hpp>
#include <mln/morpho/watershed_hierarchy.hpp>

#include <benchmark/benchmark.h>

/**
 * Simple LCA implementation, without any optimizations
 */
class SimpleLCA
{
public:
  SimpleLCA(const mln::morpho::component_tree<void>& ct)
    : t(ct)
    , d(t.compute_depth())
  {
  }

  int operator()(int a, int b) const
  {
    while (a != b)
    {
      if (d[a] < d[b])
        b = t.parent[b];
      else if (d[a] > d[b])
        a = t.parent[a];
      else
      {
        a = t.parent[a];
        b = t.parent[b];
      }
    }
    return b;
  }

private:
  const mln::morpho::component_tree<void>& t;
  const std::vector<int>                   d;
};

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

  const auto nodemap_domain = node_map.domain();
  mln_foreach (auto p, nodemap_domain)
  {
    for (auto q : mln::c4.after(p))
    {
      if (nodemap_domain.has(q))
      {
        const auto dir                              = q - p;
        res(mln::point2d{2 * p[0], 2 * p[1]} + dir) = t.values[lca(node_map(p), node_map(q))];
      }
    }
  }

  const auto res_domain = res.domain();
  mln_foreach (auto p, res_domain)
  {
    if (p[0] % 2 == 1 && p[1] % 2 == 1)
    {
      for (auto q : mln::c4(p))
      {
        if (res_domain.has(q) && res(p) < res(q))
          res(p) = res(q);
      }
    }
  }
  return res;
}

static const std::vector<std::string> filenames{"Aerial_view_of_Olbia.jpg", "Space1_20MB.jpg",
                                                "BDCN_pretrain_best_val.png"};

class BMSaliency : public benchmark::Fixture
{
public:
  BMSaliency()
  {
    if (m_trees.empty())
    {
      m_trees         = std::vector<mln::morpho::component_tree<double>>(filenames.size());
      m_nodemaps      = std::vector<mln::image2d<int>>(filenames.size());
      const auto d    = [](auto a, auto b) -> double { return mln::functional::l2dist(a, b); };
      const auto attr = [](auto tree, auto nm) {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<double>());
      };
      for (std::size_t i = 0; i < filenames.size(); i++)
      {
        mln::ndbuffer_image ima_dyn = mln::io::imread(filenames[i]);
        if (ima_dyn.sample_type() == mln::sample_type_id::UINT8)
          std::tie(m_trees[i], m_nodemaps[i]) =
              mln::morpho::watershed_hierarchy(*(ima_dyn.cast_to<std::uint8_t, 2>()), attr, mln::c4, d);
        else if (ima_dyn.sample_type() == mln::sample_type_id::RGB8)
          std::tie(m_trees[i], m_nodemaps[i]) =
              mln::morpho::watershed_hierarchy(*(ima_dyn.cast_to<mln::rgb8, 2>()), attr, mln::c4, d);
      }
    }
    this->Unit(benchmark::kSecond);
  }

  template <typename LCA>
  inline void run(benchmark::State& st, int i) const noexcept
  {
    for (auto _ : st)
      saliency<LCA>(m_trees[i], m_nodemaps[i]);
  }

protected:
  static std::vector<mln::morpho::component_tree<double>> m_trees;
  static std::vector<mln::image2d<int>>                   m_nodemaps;
};

std::vector<mln::morpho::component_tree<double>> BMSaliency::m_trees;
std::vector<mln::image2d<int>>                   BMSaliency::m_nodemaps;

BENCHMARK_F(BMSaliency, SimpleLCA_Olbia)(benchmark::State& st)
{
  run<SimpleLCA>(st, 0);
}

BENCHMARK_F(BMSaliency, LinearLCA_Olbia)(benchmark::State& st)
{
  run<mln::morpho::lca>(st, 0);
}

BENCHMARK_F(BMSaliency, SimpleLCA_Space)(benchmark::State& st)
{
  run<SimpleLCA>(st, 1);
}

BENCHMARK_F(BMSaliency, LinearLCA_Space)(benchmark::State& st)
{
  run<mln::morpho::lca>(st, 1);
}

BENCHMARK_F(BMSaliency, SimpleLCA_Map)(benchmark::State& st)
{
  run<SimpleLCA>(st, 2);
}

BENCHMARK_F(BMSaliency, LinearLCA_Map)(benchmark::State& st)
{
  run<mln::morpho::lca>(st, 2);
}

BENCHMARK_MAIN();