#include <benchmark/benchmark.h>

#include <vector>
#include <ctime>
#include <cstdlib>

#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/io/imread.hpp>

void Mult_Inplace_C(mln::image2d<mln::uint8>& img);
void Mult_Inplace_Pylene(mln::image2d<mln::uint8>& img);
void Mult_C(mln::image2d<mln::uint8>& img);
void Mult_Pylene(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_C(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_Pylene(mln::image2d<mln::uint8>& img);
void Threshold_C(mln::image2d<mln::uint8>& img);
void Threshold_Pylene(mln::image2d<mln::uint8>& img);
void LUT_Inplace_C(mln::image2d<mln::uint8>& img, std::vector<mln::uint8>& LUT);
void LUT_Inplace_Pylene(mln::image2d<mln::uint8>& img, std::vector<mln::uint8>& LUT);
void LUT_C(mln::image2d<mln::uint8>& img, std::vector<mln::uint8>& LUT);
void LUT_Pylene(mln::image2d<mln::uint8>& img, std::vector<mln::uint8>& LUT);

class Bench_Ref : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0,0);
    mln::io::imread(filename, tmp);
    resize(ima, tmp);
    mln::copy(mln::red(tmp), ima);

    srand(time(nullptr));
    mln::uint8 maxi = ~0;
    for (mln::uint8 i = 0; i < maxi; i++)
    {
      LUT.push_back(rand());
    }
    LUT.push_back(rand());
  }

protected:
  void Do_0(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima);

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_1(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& , std::vector<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima, LUT);

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  mln::image2d<mln::uint8> ima;
  std::vector<mln::uint8> LUT;
};

BENCHMARK_F(Bench_Ref, Mult_Inplace_C)(benchmark::State& st)
{
  Do_0(st, Mult_Inplace_C);
}

BENCHMARK_F(Bench_Ref, Mult_Inplace_Pylene)(benchmark::State& st)
{
  Do_0(st, Mult_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref, Mult_C)(benchmark::State& st)
{
  Do_0(st, Mult_C);
}

BENCHMARK_F(Bench_Ref, Mult_Pylene)(benchmark::State& st)
{
  Do_0(st, Mult_Pylene);
}

BENCHMARK_F(Bench_Ref, Threshold_Inplace_C)(benchmark::State& st)
{
  Do_0(st, Threshold_Inplace_C);
}

BENCHMARK_F(Bench_Ref, Threshold_Inplace_Pylene)(benchmark::State& st)
{
  Do_0(st, Threshold_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref, Threshold_C)(benchmark::State& st)
{
  Do_0(st, Threshold_C);
}

BENCHMARK_F(Bench_Ref, Threshold_Pylene)(benchmark::State& st)
{
  Do_0(st, Threshold_Pylene);
}

BENCHMARK_F(Bench_Ref, LUT_Inplace_C)(benchmark::State& st)
{
  Do_1(st, LUT_Inplace_C);
}

BENCHMARK_F(Bench_Ref, LUT_Inplace_Pylene)(benchmark::State& st)
{
  Do_1(st, LUT_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref, LUT_C)(benchmark::State& st)
{
  Do_1(st, LUT_C);
}

BENCHMARK_F(Bench_Ref, LUT_Pylene)(benchmark::State& st)
{
  Do_1(st, LUT_Pylene);
}

BENCHMARK_MAIN();
