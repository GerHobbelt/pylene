#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/experimental/imread.hpp>

#include <internal/Neighborhood/neighborhood.hpp>

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

      mln::resize(m_exp_input, in);
      mln::resize(m_exp_output, in);
      mln::transform(in, m_exp_input, [](mln::rgb8 x) -> uint8_t { return x[0]; });
      m_exp_input.extension().fill(0);

      m_exp_input.to(m_input, true);
      mln::resize(m_output, m_input);
      m_input.extension().fill(0);
      m_initialized = true;
    }
    m_h       = m_exp_input.height();
    m_w       = m_exp_input.width();
    m_stride  = m_exp_input.stride();
    m_ibuffer = m_exp_input.buffer();
    m_obuffer = m_exp_output.buffer();
  }

protected:
  using fun_t = void(*)(const mln::image2d<mln::uint8>&, mln::image2d<mln::uint8>&);
  using fun_exp_t = void(*)(const mln::experimental::image2d<mln::uint8>&, mln::experimental::image2d<mln::uint8>&);
  using fun1_t = std::function<void(const mln::image2d<mln::uint8>&, mln::image2d<mln::uint8>&)>;
  using fun1_exp_t = std::function<void(const mln::experimental::image2d<mln::uint8>&, mln::experimental::image2d<mln::uint8>&)>;
  using fun2_t = std::function<void(const mln::uint8*, mln::uint8*, int, int, std::ptrdiff_t)>;

  void run_impl(benchmark::State& st, std::function<void()> f)
  {
    while (st.KeepRunning())
      f();
    st.SetBytesProcessed(st.iterations() * m_input.domain().size());
  }

  void runit(benchmark::State& st, fun1_exp_t f) { run_impl(st, std::bind(f, m_exp_input, m_exp_output)); }
  void runit(benchmark::State& st, fun1_t f) { run_impl(st, std::bind(f, m_input, m_output)); }
  void runit(benchmark::State& st, fun2_t f) { run_impl(st, std::bind(f, m_ibuffer, m_obuffer, m_w, m_h, m_stride)); }

private:
  int                      m_w, m_h;
  std::ptrdiff_t           m_stride;
  mln::uint8*              m_ibuffer;
  mln::uint8*              m_obuffer;
  static bool                                   m_initialized;
  static mln::image2d<mln::uint8>               m_input;
  static mln::image2d<mln::uint8>               m_output;
  static mln::experimental::image2d<mln::uint8> m_exp_input;
  static mln::experimental::image2d<mln::uint8> m_exp_output;
};

bool                                   BMReferenceNeighborhood::m_initialized = false;
mln::image2d<mln::uint8>               BMReferenceNeighborhood::m_input;
mln::image2d<mln::uint8>               BMReferenceNeighborhood::m_output;
mln::experimental::image2d<mln::uint8> BMReferenceNeighborhood::m_exp_input;
mln::experimental::image2d<mln::uint8> BMReferenceNeighborhood::m_exp_output;

BENCHMARK_F(BMReferenceNeighborhood, Sum_C)(benchmark::State& st)
{
  runit(st, Sum_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Sum)(benchmark::State& st)
{
  runit(st, Sum);
}


BENCHMARK_F(BMReferenceNeighborhood, Sum_Exp_New)(benchmark::State& st)
{
  runit(st, static_cast<fun_exp_t>(Sum_New));
}


BENCHMARK_F(BMReferenceNeighborhood, Average_C)(benchmark::State& st)
{
  runit(st, Average_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Average)(benchmark::State& st)
{
  runit(st, Average);
}


BENCHMARK_F(BMReferenceNeighborhood, Average_Exp_New)(benchmark::State& st)
{
  runit(st, static_cast<fun_exp_t>(Average_New));
}


BENCHMARK_F(BMReferenceNeighborhood, Erosion_C)(benchmark::State& st)
{
  runit(st, Erosion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Erosion)(benchmark::State& st)
{
  runit(st, Erosion);
}

BENCHMARK_F(BMReferenceNeighborhood, Erosion_Exp_New)(benchmark::State& st)
{
  runit(st, static_cast<fun_exp_t>(Erosion_New));
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Isotropic_Diffusion);
}


BENCHMARK_F(BMReferenceNeighborhood, Isotropic_Diffusion_Exp_New)(benchmark::State& st)
{
  runit(st, static_cast<fun_exp_t>(Isotropic_Diffusion_New));
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion_C)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion_C);
}

BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion)(benchmark::State& st)
{
  runit(st, Anisotropic_Diffusion);
}


BENCHMARK_F(BMReferenceNeighborhood, Anisotropic_Diffusion_Exp_New)(benchmark::State& st)
{
  runit(st, static_cast<fun_exp_t>(Anisotropic_Diffusion_New));
}


BENCHMARK_MAIN();
