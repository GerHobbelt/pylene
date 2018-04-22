#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/io/imread.hpp>

#include <vector>

void Mult_Inplace_C(std::ptrdiff_t* info, mln::uint8* buffer);
void Mult_Inplace_Coro(std::ptrdiff_t* info, mln::uint8* buffer);
void Mult_Inplace_Pylene(mln::image2d<mln::uint8>& img);
void Mult_Inplace_NewWithValues(mln::image2d<mln::uint8>& img);
void Mult_Inplace_NewWithPixels(mln::image2d<mln::uint8>& img);
void Mult_C(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Mult_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Mult_NewWithValues(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Mult_NewWithPixels(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);

void Threshold_Inplace_C(std::ptrdiff_t* info, mln::uint8* buffer);
void Threshold_Inplace_Pylene(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_NewWithValues(mln::image2d<mln::uint8>& img);
void Threshold_Inplace_NewWithPixels(mln::image2d<mln::uint8>& img);
void Threshold_C(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Threshold_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Threshold_NewWithValues(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Threshold_NewWithPixels(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void LUT_Inplace_C(std::ptrdiff_t* info, mln::uint8* buffer, const mln::uint8* LUT);
void LUT_Inplace_Pylene(mln::image2d<mln::uint8>& img, const mln::uint8* LUT);
void LUT_Inplace_NewWithValues(mln::image2d<mln::uint8>& img, const mln::uint8* LUT);
void LUT_Inplace_NewWithPixels(mln::image2d<mln::uint8>& img, const mln::uint8* LUT);
void LUT_C(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* buffer_new, const mln::uint8* LUT);
void LUT_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img, const mln::uint8* LUT);
void LUT_NewWithValues(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img, const mln::uint8* LUT);
void LUT_NewWithPixels(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img, const mln::uint8* LUT);

// void Mult_Inplace_Cor(std::ptrdiff_t* info, mln::uint8* buffer);
// void LUT_Inplace_Cor(std::ptrdiff_t* info, mln::uint8* buffer, mln::uint8* LUT);

class Bench_Ref_Linear : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0, 0);
    mln::io::imread(filename, tmp);
    resize(ima, tmp);
    mln::copy(mln::red(tmp), ima);

    mln::resize(out_ima, ima);

    srand(time(nullptr));
    mln::uint8 maxi = ~0;
    for (int i = 0; i < maxi; i++)
    {
      LUT.push_back(rand());
    }
    LUT.push_back(rand());

    info[0] = ima.nrows();
    info[1] = ima.ncols();
    info[2] = ima.strides()[0];
  }

protected:
  void Do_Pylene_0(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima);
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_Pylene_1(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& , const mln::uint8*))
  {
    while (st.KeepRunning())
      func(ima, LUT.data());
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_Pylene_2(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& , mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima, out_ima);
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_Pylene_3(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& , mln::image2d<mln::uint8>& , const mln::uint8*))
  {
    while (st.KeepRunning())
      func(ima, out_ima, LUT.data());
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_C_0(benchmark::State& st, void (*func)(std::ptrdiff_t*, mln::uint8*))
  {
    while (st.KeepRunning())
      func(info, &ima.at(0,0));
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_C_1(benchmark::State& st, void (*func)(std::ptrdiff_t*, mln::uint8*, const mln::uint8*))
  {
    while (st.KeepRunning())
      func(info, &ima.at(0,0), LUT.data());
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_C_2(benchmark::State& st, void (*func)(std::ptrdiff_t*, mln::uint8*, mln::uint8*))
  {
    while (st.KeepRunning())
      func(info, &ima.at(0,0), &out_ima.at(0,0));
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

  void Do_C_3(benchmark::State& st, void (*func)(std::ptrdiff_t*, mln::uint8*, mln::uint8*, const mln::uint8*))
  {
    while (st.KeepRunning())
      func(info, &ima.at(0,0), &out_ima.at(0,0), LUT.data());
    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  std::ptrdiff_t info[3];
  mln::image2d<mln::uint8> ima;
  mln::image2d<mln::uint8> out_ima;
  std::vector<mln::uint8> LUT;
};

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_C)(benchmark::State& st)
{
  Do_C_0(st, Mult_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_Coro)(benchmark::State& st)
{
  Do_C_0(st, Mult_Inplace_Coro);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_Pylene)(benchmark::State& st)
{
  Do_Pylene_0(st, Mult_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_0(st, Mult_Inplace_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_0(st, Mult_Inplace_NewWithPixels);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_C)(benchmark::State& st)
{
  Do_C_2(st, Mult_C);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_Pylene)(benchmark::State& st)
{
  Do_Pylene_2(st, Mult_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_2(st, Mult_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, Mult_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_2(st, Mult_NewWithPixels);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_C)(benchmark::State& st)
{
  Do_C_0(st, Threshold_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_Pylene)(benchmark::State& st)
{
  Do_Pylene_0(st, Threshold_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_0(st, Threshold_Inplace_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Inplace_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_0(st, Threshold_Inplace_NewWithPixels);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_C)(benchmark::State& st)
{
  Do_C_2(st, Threshold_C);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_Pylene)(benchmark::State& st)
{
  Do_Pylene_2(st, Threshold_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_2(st, Threshold_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, Threshold_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_2(st, Threshold_NewWithPixels);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_C)(benchmark::State& st)
{
  Do_C_1(st, LUT_Inplace_C);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_Pylene)(benchmark::State& st)
{
  Do_Pylene_1(st, LUT_Inplace_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_1(st, LUT_Inplace_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_1(st, LUT_Inplace_NewWithPixels);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_C)(benchmark::State& st)
{
  Do_C_3(st, LUT_C);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_Pylene)(benchmark::State& st)
{
  Do_Pylene_3(st, LUT_Pylene);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_NewWithValues)(benchmark::State& st)
{
  Do_Pylene_3(st, LUT_NewWithValues);
}

BENCHMARK_F(Bench_Ref_Linear, LUT_NewWithPixels)(benchmark::State& st)
{
  Do_Pylene_3(st, LUT_NewWithPixels);
}

// BENCHMARK_F(Bench_Ref_Linear, Mult_Inplace_Cor)(benchmark::State& st)
// {
//   Do_C_0(st, Mult_Inplace_Cor);
// }

// BENCHMARK_F(Bench_Ref_Linear, LUT_Inplace_Cor)(benchmark::State& st)
// {
//   Do_C_1(st, LUT_Inplace_Cor);
// }

BENCHMARK_MAIN();
