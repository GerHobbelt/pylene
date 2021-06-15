#include <mln/io/imread.hpp>

#include <benchmark/benchmark.h>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/morpho/alphatree.hpp>

using image_t = mln::image2d<mln::rgb8>;

static const std::vector<std::string> bench_images = {"plane.png", "nature.png", "mountains.png", "hong_kong.png"};

static const auto alphatree_vect_function = [](const image_t& input) {
  mln::morpho::alphatree(input, mln::c4,
                         [](const auto& a, const auto& b) -> std::double_t { return mln::l2dist(a, b); });
};

static const auto alphatree_hq_function = [](const image_t& input) {
  mln::morpho::alphatree(input, mln::c4,
                         [](const auto& a, const auto& b) -> std::uint16_t { return mln::l2dist(a, b); });
};

class BMAlphaTree : public benchmark::Fixture
{
public:
  BMAlphaTree()
  {
    if (inputs.empty())
    {
      inputs = std::vector<image_t>(bench_images.size());
      for (std::size_t i = 0; i < bench_images.size(); ++i)
        mln::io::imread(bench_images[i], inputs[i]);
    }
  }

  void run(benchmark::State& st, const std::function<void(const image_t& input)>& callback, std::size_t i) const
  {
    for (auto _ : st)
      callback(inputs[i]);
  }

  std::vector<image_t> inputs;
};


BENCHMARK_F(BMAlphaTree, AlphatreePlaneVect)(benchmark::State& st)
{
  this->run(st, alphatree_vect_function, 0);
}

BENCHMARK_F(BMAlphaTree, AlphatreePlaneHQ)(benchmark::State& st)
{
  this->run(st, alphatree_hq_function, 0);
}

BENCHMARK_F(BMAlphaTree, AlphatreeNatureVect)(benchmark::State& st)
{
  this->run(st, alphatree_vect_function, 1);
}

BENCHMARK_F(BMAlphaTree, AlphatreeNatureHQ)(benchmark::State& st)
{
  this->run(st, alphatree_hq_function, 1);
}

BENCHMARK_F(BMAlphaTree, AlphatreeMountainsVect)(benchmark::State& st)
{
  this->run(st, alphatree_vect_function, 2);
}

BENCHMARK_F(BMAlphaTree, AlphatreeMountainsHQ)(benchmark::State& st)
{
  this->run(st, alphatree_hq_function, 2);
}

BENCHMARK_F(BMAlphaTree, AlphatreeHongKongVect)(benchmark::State& st)
{
  this->run(st, alphatree_vect_function, 3);
}

BENCHMARK_F(BMAlphaTree, AlphatreeHongKongHQ)(benchmark::State& st)
{
  this->run(st, alphatree_hq_function, 3);
}

BENCHMARK_MAIN();