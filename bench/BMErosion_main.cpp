#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <benchmark/benchmark.h>

#include "BMErosion.hpp"

using namespace mln;
#define MLN_IMG_PATH "../../img/"

struct BMErosion : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "lena.pgm";

  BMErosion()
  {
    io::imread(filename, m_input);
    int nr = m_input.nrows();
    int nc = m_input.ncols();
    resize(m_output, m_input);
    m_bytes = nr * nc * sizeof(uint8);
    mln::extension::fill(m_input, 255);
  }

  image2d<uint8> m_input;
  image2d<uint8> m_output;
  std::size_t    m_bytes;
};

BENCHMARK_F(BMErosion, Erosion_CStyle_Uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    erode_c8_cstyle(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMErosion, Erosion_CStyle_Restrict_Uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    erode_c8_cstyle_restrict(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMErosion, Erosion_Iterators_Uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    erode_c8_mlnstyle(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMErosion, Erosion_Kernel_Uint8)(benchmark::State& st)
{
  while (st.KeepRunning())
  {
    erode_c8_kernel(m_input, m_output);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_MAIN();
