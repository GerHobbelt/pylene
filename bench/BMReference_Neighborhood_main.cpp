
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/experimental/imread.hpp>

#include <bench/neighborhood.hpp>

#include <benchmark/benchmark.h>


class BMReferenceNeighborhood : public benchmark::Fixture
{
  static constexpr const char* filename = "Space1_20MB.jpg";

  virtual void SetUp(const benchmark::State&) override
  {
    if (!m_initialized)
    {
      mln::experimental::image2d<mln::rgb8> in;
      mln::io::experimental::imread(filename, in);

      mln::resize(m_input, in);
      mln::resize(m_output, in);
      mln::transform(in, m_input, [](mln::rgb8 x) -> uint8_t { return x[0]; });
      m_input.extension().fill(0);
      m_initialized = true;
    }
    m_h       = m_input.height();
    m_w       = m_input.width();
    m_stride  = m_input.stride();
    m_ibuffer = m_input.buffer();
    m_obuffer = m_output.buffer();
  }

protected:
  using fun1_t = std::function<void(const mln::experimental::image2d<uint8_t>&, mln::experimental::image2d<uint8_t>&)>;
  using fun2_t = std::function<void(const uint8_t*, uint8_t*, int, int, std::ptrdiff_t)>;

  void run_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun1_t f) { run_impl(st, std::bind(f, m_input, m_output)); }
  void runit(benchmark::State& st, fun2_t f) { run_impl(st, std::bind(f, m_ibuffer, m_obuffer, m_w, m_h, m_stride)); }

private:
  int                                        m_w, m_h;
  std::ptrdiff_t                             m_stride;
  uint8_t*                                   m_ibuffer;
  uint8_t*                                   m_obuffer;
  static bool                                m_initialized;
  static mln::experimental::image2d<uint8_t> m_input;
  static mln::experimental::image2d<uint8_t> m_output;
};

bool                                BMReferenceNeighborhood::m_initialized = false;
mln::experimental::image2d<uint8_t> BMReferenceNeighborhood::m_input;
mln::experimental::image2d<uint8_t> BMReferenceNeighborhood::m_output;

BENCHMARK_F(BMReferenceNeighborhood, Sum_C)(benchmark::State& st)
{
  runit(st, Sum_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Sum)(benchmark::State& st)
{
  runit(st, Sum);
}


BENCHMARK_F(BMReferenceNeighborhood, Average_C)(benchmark::State& st)
{
  runit(st, Average_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Average)(benchmark::State& st)
{
  runit(st, Average);
}


BENCHMARK_F(BMReferenceNeighborhood, Erosion_C)(benchmark::State& st)
{
  runit(st, Erosion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Erosion)(benchmark::State& st)
{
  runit(st, Erosion);
}


BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion);
}



BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion);
}


BENCHMARK_MAIN();
