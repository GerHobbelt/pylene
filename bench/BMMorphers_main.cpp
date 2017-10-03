#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <benchmark/benchmark.h>


unsigned threshold1(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold1_bis(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold2(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold3(const mln::image2d<mln::uint8>& f, mln::uint8 v);
unsigned threshold4(const mln::image2d<mln::uint8>& f, mln::uint8 v);
void threshold5(const mln::image2d<mln::uint8>& f, mln::image2d<bool>& out, mln::uint8 v);
void threshold6(const mln::image2d<mln::uint8>& f, mln::image2d<bool>& out, mln::uint8 v);

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

protected:
  void Do_0(benchmark::State& st, unsigned (*func)(const image2d<mln::uint8>&, mln::uint8))
  {
    auto input = eval(red(m_input));
    unsigned count;
    while (st.KeepRunning())
      count = func(input, 128);

    std::cout << count << std::endl;
    st.SetBytesProcessed(st.iterations() * m_bytes);
  }

  void Do_1(benchmark::State& st, void (*func)(const image2d<mln::uint8>&, image2d<bool>&, mln::uint8))
  {
    auto input = eval(red(m_input));
    while (st.KeepRunning())
      func(input, m_output, 128);

    st.SetBytesProcessed(st.iterations() * m_bytes);
  }

private:
  image2d<rgb8> m_input;
  image2d<bool> m_output;
  std::size_t   m_bytes;
};



BENCHMARK_F(BMMorphers, Threshold_Count_CStyle_Uint8)(benchmark::State& st) { Do_0(st, threshold1); }
BENCHMARK_F(BMMorphers, Threshold_Count_CStyle2_Uint8)(benchmark::State& st) { Do_0(st, threshold1_bis); }
BENCHMARK_F(BMMorphers, Threshold_Count_ValueIterator_Uint8)(benchmark::State& st) { Do_0(st, threshold2); }
BENCHMARK_F(BMMorphers, Threshold_Count_PixelIterator_Uint8)(benchmark::State& st) { Do_0(st, threshold3); }
BENCHMARK_F(BMMorphers, Threshold_Count_Morpher1)(benchmark::State& st) { Do_0(st, threshold4); }
BENCHMARK_F(BMMorphers, Threshold_Out_Native)(benchmark::State& st) { Do_1(st, threshold5); }
BENCHMARK_F(BMMorphers, Threshold_Out_Morpher)(benchmark::State& st) { Do_1(st, threshold6); }

BENCHMARK_MAIN();
