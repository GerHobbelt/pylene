#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/ptroffset.hpp>
#include <mln/io/imread.hpp>
#include <benchmark/benchmark.h>

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

// Threshold A la C
template <class T>
unsigned
__attribute__ ((noinline)) 
threshold1(const image2d<T>& f, image2d<bool>& out, T v)
{
  int istride = f.strides()[0];
  int ostride = out.strides()[0];
  int nr = f.nrows();
  int nc = f.ncols();
  const T* ptr_in = & f({0,0});
  bool*    ptr_out = &out({0,0});
  unsigned count = 0;

  for (int i = 0; i < nr; ++i) {
    for (int j = 0; j < nc; ++j) {
      //ptr_out[j] = ptr_in[j] < v;
      count += ptr_in[j] < v;
    }
    ptr_in = ptr_offset(ptr_in, istride);
    //ptr_out = ptr_offset(ptr_out, ostride);
  }
  return count;
}

// Threshold A la C
template <class T>
unsigned
__attribute__ ((noinline))
threshold2(const image2d<T>& f, image2d<bool>& out, T v)
{
  int istride[2] = {(int)f.strides()[0], (int)f.strides()[1]};
  int ostride[2] = {(int)out.strides()[0], (int)out.strides()[1]};
  int nr = f.nrows();
  int nc = f.ncols();
  const T* ptr_in = & f({0,0});
  bool*    ptr_out = &out({0,0});
  unsigned count = 0;

  for (int i = 0; i < nr; ++i) {
    auto in = ptr_in;
    auto out = ptr_out;
    for (int j = 0; j < nc; ++j) {
      count += *in < v;
      //*out = *in < v;
      //out = ptr_offset(out, ostride[1]);
      in = ptr_offset(in, istride[1]);
    }
    ptr_in = ptr_offset(ptr_in, istride[0]);
    //ptr_out = ptr_offset(ptr_out, ostride[0]);
  }
  return count;
}

template <class T>
unsigned
__attribute__ ((noinline))
threshold3(const image2d<T>& f, image2d<bool>& out, T v)
{
  auto g = f < v;
  unsigned count = 0;
  mln_foreach(auto v, g.values()) {
    count += (bool)v;
  }
  return count;
}


BENCHMARK_F(BMMorphers, Threshold_CStyle_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(threshold1(input, m_output, (uint8)128));
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_F(BMMorphers, Threshold_CStyle2_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  while (st.KeepRunning()) {
    benchmark::DoNotOptimize(threshold2(input, m_output, (uint8)128));
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}


BENCHMARK_F(BMMorphers, Threshold_Morpher_Uint8)(benchmark::State& st) {
  auto input = eval(red(m_input));
  while (st.KeepRunning()) {
    threshold3(input, m_output, (uint8)128);
  }
  st.SetBytesProcessed(st.iterations() * m_bytes);
}

BENCHMARK_MAIN();
