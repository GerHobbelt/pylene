#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>

class Bench_Ref_Neighborhood : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0,0);
    mln::io::imread(filename, tmp);
    resize(ima, tmp);
    mln::copy(mln::red(tmp), ima);
  }

protected:
  void Do_0(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima);

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  mln::image2d<mln::uint8> ima;
};

BENCHMARK_MAIN();