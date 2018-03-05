#include <benchmark/benchmark.h>

#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/core/extension/fill.hpp>

void Sum_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Sum_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Average_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Average_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Erosion_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Erosion_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);
void Isotropic_Diffusion_C(int* info, mln::uint8* buffer, mln::uint8* buffer_new);
void Isotropic_Diffusion_Pylene(mln::image2d<mln::uint8>& img, mln::image2d<mln::uint8>& new_img);

class Bench_Ref_Neighborhood : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0,0);
    mln::io::imread(filename, tmp);
    resize(ima, tmp);
    mln::copy(mln::red(tmp), ima);

    mln::resize(out_ima, ima);
    mln::extension::fill(ima, 0);

    info[0] = ima.nrows();
    info[1] = ima.ncols();
    info[2] = ima.strides()[0];
  }

protected:
  void Do_0(benchmark::State& st, void (*func)(mln::image2d<mln::uint8>& , mln::image2d<mln::uint8>& ))
  {
    while (st.KeepRunning())
      func(ima, out_ima);

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }
  
  void Do_1(benchmark::State& st, void (*func)(int* info, mln::uint8* , mln::uint8* ))
  {
    while (st.KeepRunning())
      func(info, &ima.at(0,0), &out_ima.at(0,0));

    st.SetBytesProcessed(st.iterations() * ima.domain().size() * sizeof(int));
  }

private:
  int info[3];
  mln::image2d<mln::uint8> ima;
  mln::image2d<mln::uint8> out_ima;
};

BENCHMARK_F(Bench_Ref_Neighborhood, Sum_C)(benchmark::State& st)
{
  Do_1(st, Sum_C);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Sum_Pylene)(benchmark::State& st)
{
  Do_0(st, Sum_Pylene);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Average_C)(benchmark::State& st)
{
  Do_1(st, Average_C);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Average_Pylene)(benchmark::State& st)
{
  Do_0(st, Average_Pylene);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Erosion_C)(benchmark::State& st)
{
  Do_1(st, Erosion_C);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Erosion_Pylene)(benchmark::State& st)
{
  Do_0(st, Erosion_Pylene);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Isotropic_Diffusion_C)(benchmark::State& st)
{
  Do_1(st, Isotropic_Diffusion_C);
}

BENCHMARK_F(Bench_Ref_Neighborhood, Isotropic_Diffusion_Pylene)(benchmark::State& st)
{
  Do_0(st, Isotropic_Diffusion_Pylene);
}

BENCHMARK_MAIN();