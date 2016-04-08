#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/neighborhood/sliding_viter.hpp>
#include <mln/core/wrt_offset.hpp>
#include <mln/core/algorithm/iota.hpp>

using namespace mln;

class Bench_Neighborhood: public benchmark::Fixture
{
  virtual void SetUp(const benchmark::State&) override
  {
    const int nrows = 1000, ncols = 10000;
    ima = image2d<int>(nrows, ncols);
    iota(ima, 0);
  }

protected:
  image2d<int> ima;
};

BENCHMARK_F(Bench_Neighborhood, pixter)(benchmark::State& st)
{
  double u;
  while (st.KeepRunning())
    {
      mln_pixter(px, ima);
      mln_iter(nx, c8(px));

      u = 0;
      mln_forall(px)
        mln_forall(nx)
          benchmark::DoNotOptimize(u += nx->val());
    }
  std::cout << u << std::endl;
  (void) u;
}

BENCHMARK_F(Bench_Neighborhood, viter)(benchmark::State& st)
{
  double u;
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

BENCHMARK_F(Bench_Neighborhood, piter)(benchmark::State& st)
{
  double u;
  while (st.KeepRunning())
    {
      mln_iter(p, ima.domain());
      mln_iter(n, c8(p));

      u = 0;
      mln_forall(p)
        mln_forall(n)
          benchmark::DoNotOptimize(u += ima.at(*n));
    }
  std::cout << u << std::endl;
  (void) u;
}

BENCHMARK_F(Bench_Neighborhood, indexes)(benchmark::State& st)
{
  double u;
  while (st.KeepRunning())
    {
      std::size_t idx = ima.index_of_point(ima.domain().pmin);
      auto w = wrt_delta_index(ima, c8_t::dpoints);
      mln_iter(p, ima.domain());
      mln_iter(n, c8(p));

      u = 0;
      unsigned nrows = ima.nrows();
      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < nrows; ++i)
        {
          for (unsigned j = 0; j < ncols; ++j)
            {
              std::size_t p = idx + j;// * ima.index_strides()[1];
              mln_foreach(auto k, w) {
                benchmark::DoNotOptimize(u += ima[p + k]);
              }
            }
          idx += ima.index_strides()[0];
        }
    }
  std::cout << u << std::endl;
  (void) u;
}

BENCHMARK_F(Bench_Neighborhood, pointers)(benchmark::State& st)
{
  double u;
  while (st.KeepRunning())
    {
      constexpr int sz = 8;
      auto dpoints = c8_t::dpoints;

      const char* ptr2 = (char*) &ima(ima.domain().pmin);
      const size_t strides[2] = {ima.strides()[0], ima.strides()[1]};
      const point2d pmax = ima.domain().shape();
      ptrdiff_t offsets[8];
      wrt_offset(ima, dpoints, offsets);

      u = 0;
      for (int x = 0; x < pmax[0]; ++x, ptr2 += strides[0])
        {
          const char* ptr = ptr2;
          for (int y = 0; y < pmax[1]; ++y, ptr += strides[1])
            for (int k = 0; k < sz; ++k)
              benchmark::DoNotOptimize(u += *(const int*)(ptr + offsets[k]));
        }
    }
  std::cout << u << std::endl;
  (void) u;
}

BENCHMARK_MAIN();
