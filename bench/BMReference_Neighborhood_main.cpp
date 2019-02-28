#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/Neighborhood/neighborhood.hpp>

#include <benchmark/benchmark.h>


class BMReferenceNeighborhood : public benchmark::Fixture
{
  static constexpr const char* filename = "Space1_20MB.jpg";

  virtual void SetUp(const benchmark::State&) override
  {
    mln::image2d<mln::rgb8> tmp(0, 0);
    mln::io::imread(filename, tmp);

    mln::resize(m_input, tmp);
    mln::resize(m_output, tmp);
    mln::copy(mln::red(tmp), m_input);
    mln::extension::fill(m_input, 0);

    m_h       = m_input.nrows();
    m_w       = m_input.ncols();
    m_stride  = m_input.strides()[0];
    m_ibuffer = &m_input.at(0, 0);
    m_obuffer = &m_output.at(0, 0);
  }

protected:
  using fun1_t = std::function<void(const mln::image2d<mln::uint8>&, mln::image2d<mln::uint8>&)>;
  using fun2_t = std::function<void(const mln::uint8*, mln::uint8*, int, int, std::ptrdiff_t)>;

  void run_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun1_t f) { run_impl(st, std::bind(f, m_input, m_output)); }
  void runit(benchmark::State& st, fun2_t f) { run_impl(st, std::bind(f, m_ibuffer, m_obuffer, m_w, m_h, m_stride)); }

private:
  int                      m_w, m_h;
  std::ptrdiff_t           m_stride;
  mln::uint8*              m_ibuffer;
  mln::uint8*              m_obuffer;
  mln::image2d<mln::uint8> m_input;
  mln::image2d<mln::uint8> m_output;
};

BENCHMARK_F(BMReferenceNeighborhood, Sum_C)(benchmark::State& st)
{
  runit(st, Sum_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Sum)(benchmark::State& st)
{
  runit(st, Sum);
}

BENCHMARK_F(BMReferenceNeighborhood, Sum_New)(benchmark::State& st)
{
  runit(st, Sum_New);
}


BENCHMARK_F(BMReferenceNeighborhood, Average_C)(benchmark::State& st)
{
  runit(st, Average_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Average)(benchmark::State& st)
{
  runit(st, Average);
}

BENCHMARK_F(BMReferenceNeighborhood, Average_New)(benchmark::State& st)
{
  runit(st, Average_New);
}


BENCHMARK_F(BMReferenceNeighborhood, Erosion_C)(benchmark::State& st)
{
  runit(st, Erosion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Erosion)(benchmark::State& st)
{
  runit(st, Erosion);
}

BENCHMARK_F(BMReferenceNeighborhood, Erosion_New)(benchmark::State& st)
{
  runit(st, Erosion_New);
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion);
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion_New)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion_New);
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion);
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion_New)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion_New);
}


BENCHMARK_MAIN();
