#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/io/imread.hpp>

void Mult_Inplace_C(mln::image2d<mln::uint8>& img);
void Mult_Inplace_Pylene(mln::image2d<mln::uint8>& img);
void Threshold_C(mln::image2d<mln::uint8>& img);
void Threshold_Pylene(mln::image2d<mln::uint8>& img);

class Bench_Ref_Linear : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0, 0);
    mln::io::imread(filename, tmp);
    resize(ima, tmp);
    mln::copy(mln::red(tmp), ima);
  }

protected:
  void Do(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima);

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  mln::image2d<mln::uint8> ima;
};

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_C)(benchmark::State& st)
{
  Do(st, Mult_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_Pylene)(benchmark::State& st)
{
  Do(st, Mult_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_C)(benchmark::State& st)
{
  Do(st, Threshold_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Pylene)(benchmark::State& st)
{
  Do(st, Threshold_Pylene);
}

BENCHMARK_MAIN();
