#include <mln/core/extension/fill.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <benchmark/benchmark.h>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/structural/dilate.hpp>

using namespace mln;
#define MLN_IMG_PATH "../../img"

class BMDilation : public benchmark::Fixture
{
public:
  BMDilation()
  {
    io::imread(MLN_IMG_PATH "/lena.pgm", m_input);
    int nr = m_input.nrows();
    int nc = m_input.ncols();
    resize(m_output, m_input);
    m_bytes = nr * nc * sizeof(uint8);
  }

  template <class SE, class Compare = mln::productorder_less<mln::uint8>>
  void run(benchmark::State& st, const SE& se, Compare cmp = Compare())
  {
    for (auto _ : st)
      mln::morpho::structural::dilate(m_input, se, m_output, cmp);
    st.SetBytesProcessed(st.iterations() * m_bytes);
  }

protected:
  image2d<uint8> m_input;
  image2d<uint8> m_output;
  std::size_t    m_bytes;
};

BENCHMARK_DEFINE_F(BMDilation, EuclideanDisc_naive)(benchmark::State& st)
{
  int           radius = st.range(0);
  mln::se::disc se(radius, 0);
  this->run(st, se, std::less<mln::uint8>());
}

BENCHMARK_DEFINE_F(BMDilation, EuclideanDisc_incremental)(benchmark::State& st)
{
  int           radius = st.range(0);
  mln::se::disc se(radius, 0);
  this->run(st, se);
}

BENCHMARK_DEFINE_F(BMDilation, ApproximatedDisc)(benchmark::State& st)
{
  int           radius = st.range(0);
  mln::se::disc se(radius);
  this->run(st, se);
}


BENCHMARK_DEFINE_F(BMDilation, Square)(benchmark::State& st)
{
  int             width = 2 * st.range(0) + 1;
  mln::se::rect2d se(width, width);
  this->run(st, se);
}

constexpr int max_range = 2 << 6;

BENCHMARK_REGISTER_F(BMDilation, ApproximatedDisc)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, EuclideanDisc_naive)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, EuclideanDisc_incremental)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, Square)->RangeMultiplier(2)->Range(2, max_range);


BENCHMARK_MAIN();
