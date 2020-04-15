#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/experimental/imread.hpp>

#include <mln/core/colors.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <benchmark/benchmark.h>

unsigned threshold1(mln::experimental::image2d<uint8_t> f, uint8_t v);
unsigned threshold1_bis(mln::experimental::image2d<uint8_t> f, uint8_t v);
unsigned threshold2(mln::experimental::image2d<uint8_t> f, uint8_t v);
unsigned threshold3(mln::experimental::image2d<uint8_t> f, uint8_t v);
unsigned threshold4(mln::experimental::image2d<uint8_t> f, uint8_t v);
void     threshold5(mln::experimental::image2d<uint8_t> f, mln::experimental::image2d<bool> out, uint8_t v);
void     threshold6(mln::experimental::image2d<uint8_t> f, mln::experimental::image2d<bool> out, uint8_t v);

using namespace mln;

struct BMMorphers : public benchmark::Fixture
{
  std::string filename = fixtures::ImagePath::concat_with_filename("lena.ppm");

  BMMorphers()
  {
    mln::io::experimental::imread(filename, m_input);
    int nr = m_input.height();
    int nc = m_input.width();
    resize(m_output, m_input);
    m_bytes = nr * nc * sizeof(rgb8);
  }

protected:
  void Do_0(benchmark::State& st, unsigned (*func)(mln::experimental::image2d<uint8_t>, uint8_t))
  {
    auto input = mln::transform(m_input, [](auto x) { return x[0]; });

    unsigned count;
    while (st.KeepRunning())
      count = func(input, 128);

    std::cout << count << std::endl;
    st.SetBytesProcessed(st.iterations() * m_bytes);
  }

  void Do_1(benchmark::State& st, void (*func)(mln::experimental::image2d<uint8_t>, mln::experimental::image2d<bool>, uint8_t))
  {
    auto input = mln::transform(m_input, [](auto x) { return x[0]; });

    while (st.KeepRunning())
      func(input, m_output, 128);

    st.SetBytesProcessed(st.iterations() * m_bytes);
  }

private:
  mln::experimental::image2d<mln::rgb8> m_input;
  mln::experimental::image2d<bool>      m_output;
  std::size_t   m_bytes;
};

BENCHMARK_F(BMMorphers, Threshold_Count_CStyle_Uint8)(benchmark::State& st)
{
  Do_0(st, threshold1);
}
BENCHMARK_F(BMMorphers, Threshold_Count_CStyle2_Uint8)(benchmark::State& st)
{
  Do_0(st, threshold1_bis);
}
BENCHMARK_F(BMMorphers, Threshold_Count_ValueIterator_Uint8)(benchmark::State& st)
{
  Do_0(st, threshold2);
}
BENCHMARK_F(BMMorphers, Threshold_Count_PixelIterator_Uint8)(benchmark::State& st)
{
  Do_0(st, threshold3);
}
BENCHMARK_F(BMMorphers, Threshold_Count_Morpher1)(benchmark::State& st)
{
  Do_0(st, threshold4);
}
BENCHMARK_F(BMMorphers, Threshold_Out_Native)(benchmark::State& st)
{
  Do_1(st, threshold5);
}
BENCHMARK_F(BMMorphers, Threshold_Out_Morpher)(benchmark::State& st)
{
  Do_1(st, threshold6);
}

BENCHMARK_MAIN();
