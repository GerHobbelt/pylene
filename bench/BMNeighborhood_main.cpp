
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <benchmark/benchmark.h>

using namespace mln;

long bench_pixter_0(const image2d<int>& ima);
long bench_pixter(const image2d<int>& ima);
long bench_piter(const image2d<int>& ima);
long bench_indexes(const image2d<int>& ima);
long bench_pointers(const image2d<int>& ima);

class BMNeighborhood : public benchmark::Fixture
{
  virtual void SetUp(const benchmark::State&) override
  {
    const int nrows = 1000, ncols = 10000;
    ima = image2d<int>(nrows, ncols);
    iota(ima, 0);
  }

protected:
  void Do(benchmark::State& st, long (*func)(const image2d<int>&))
  {
    long u;
    while (st.KeepRunning())
      u = func(ima);

    std::cout << u << std::endl;
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  image2d<int> ima;
};

BENCHMARK_F(BMNeighborhood, pixter_0)(benchmark::State& st)
{
  Do(st, bench_pixter_0);
}
BENCHMARK_F(BMNeighborhood, pixter)(benchmark::State& st)
{
  Do(st, bench_pixter);
}
BENCHMARK_F(BMNeighborhood, piter)(benchmark::State& st)
{
  Do(st, bench_piter);
}
BENCHMARK_F(BMNeighborhood, indexes)(benchmark::State& st)
{
  Do(st, bench_indexes);
}
BENCHMARK_F(BMNeighborhood, pointers)(benchmark::State& st)
{
  Do(st, bench_pointers);
}

/*
BENCHMARK_F(BMNeighborhood, viter)(benchmark::State& st)
{
  long u;
  while (st.KeepRunning())
    {
      mln_pixter(px, ima);
      auto nv = make_sliding_viter(px, c8.dpoints);

      u = 0;
      mln_forall(px)
        mln_forall(nv)
          benchmark::DoNotOptimize(u += *nv);
    }
  std::cout << u << std::endl;
  (void) u;
}
*/

BENCHMARK_MAIN();
