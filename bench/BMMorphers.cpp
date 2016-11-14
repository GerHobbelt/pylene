#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/ptroffset.hpp>
#include <mln/io/imread.hpp>
#include <benchmark/benchmark.h>
#include "standalone.hpp"
using namespace mln;

struct BMMorphers : public benchmark::Fixture
{
  const char* filename = MLN_IMG_PATH "/lena.ppm";

  BMMorphers()
  {
    io::imread(filename, m_input);
    int nr = m_input.nrows();
    int nc = m_input.ncols();
    resize(m_output, m_input);
    m_bytes = nr * nc * sizeof(rgb8);
  }


  image2d<rgb8> m_input;
  image2d<bool> m_output;
  std::size_t   m_bytes;
};



BENCHMARK_F(BMMorphers, Threshold_Count_CStyle_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  unsigned count;
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(count = threshold1(input, (uint8)128));
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Count_CStyle2_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  unsigned count;
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(count = threshold1_bis(input, (uint8)128));
  }
  std::cout << count << "\n";
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Count_ValueIterator_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  unsigned count;
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(count = threshold2(input, (uint8)128));
  }
    std::cout << count << "\n";
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Count_PixelIterator_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  unsigned count;
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(count = threshold3(input, (uint8)128));
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Count_Morpher1)(benchmark::State& st) {
  auto input = eval(red(m_input));
  unsigned count;
  while (st.KeepRunning()) {
    count = threshold4(input, (uint8)128);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Out_Native)(benchmark::State& st) {
  auto input = eval(red(m_input));
  while (st.KeepRunning()) {
    threshold5(input, m_output, (uint8)128);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_Out_Morpher)(benchmark::State& st) {
  auto input = eval(red(m_input));
  while (st.KeepRunning()) {
    threshold6(input, m_output, (uint8)128);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}


BENCHMARK_MAIN();
