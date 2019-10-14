#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/neighb2d.hpp>

#include <mln/io/experimental/imread.hpp>

#include <mln/morpho/hit_or_miss.hpp>
#include <mln/morpho/median_filter.hpp>
#include <mln/morpho/opening_by_reconstruction.hpp>
#include <mln/morpho/structural/closing.hpp>
#include <mln/morpho/structural/dilate.hpp>
#include <mln/morpho/structural/erode.hpp>
#include <mln/morpho/structural/opening.hpp>
#include <mln/morpho/watershed.hpp>

#include <mln/labeling/local_extrema.hpp>

// [legacy]
#include <mln/core/image/image2d.hpp>

#include <benchmark/benchmark.h>


class BMMorpho : public benchmark::Fixture
{
public:
  using image_t = mln::image2d<uint8_t>;

  BMMorpho()
  {
    if (!g_loaded)
    {
      const char* filename = "Aerial_view_of_Olbia.jpg";
      mln::experimental::image2d<mln::rgb8> input;
      mln::io::experimental::imread(filename, input);

      g_input = mln::transform(input, [](mln::rgb8 x) -> uint8_t { return x[0]; });
      g_loaded = true;
    }
    m_input = g_input;
    int nr = m_input.width();
    int nc = m_input.height();
    mln::resize(m_output, m_input);
    m_size = nr * nc;

    m_input.to(m_input_, false);
    m_output.to(m_output_, false);
  }

  void run(benchmark::State& st, std::function<void(const image_t& input, image_t& output)> callback)
  {
    for (auto _ : st)
      callback(m_input_, m_output_);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  static bool                                g_loaded;
  static mln::experimental::image2d<uint8_t> g_input;

  mln::experimental::image2d<uint8_t> m_input;
  mln::experimental::image2d<uint8_t> m_output;
  mln::image2d<uint8_t>               m_input_;
  mln::image2d<uint8_t>               m_output_;
  std::size_t                         m_size;
};

bool                                BMMorpho::g_loaded = false;
mln::experimental::image2d<uint8_t> BMMorpho::g_input;



BENCHMARK_DEFINE_F(BMMorpho, Dilation_EuclideanDisc_incremental)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::se::disc(radius, 0);

  auto f = [se](const image_t& input, image_t& output) { mln::morpho::structural::dilate(input, se, output, mln::productorder_less<uint8_t>()); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMMorpho, Dilation_ApproximatedDisc)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::se::disc(radius, 8);

  auto f = [se](const image_t& input, image_t& output) { mln::morpho::structural::dilate(input, se, output, mln::productorder_less<uint8_t>()); };
  this->run(st, f);
}


BENCHMARK_DEFINE_F(BMMorpho, Dilation_Square)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::se::rect2d(2 * radius + 1, 2 * radius + 1);
  auto f      = [se](const image_t& input, image_t& output) { mln::morpho::structural::dilate(input, se, output, mln::productorder_less<uint8_t>()); };
  this->run(st, f);
}

constexpr int max_range = 128;

BENCHMARK_REGISTER_F(BMMorpho, Dilation_ApproximatedDisc)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMMorpho, Dilation_EuclideanDisc_incremental)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMMorpho, Dilation_Square)->RangeMultiplier(2)->Range(2, max_range);



BENCHMARK_F(BMMorpho, Opening_Disc)(benchmark::State& st)
{
  int  radius  = 32;
  auto se      = mln::se::rect2d(2 * radius + 1, 2 * radius + 1);
  auto f       = [se](const image_t& input, image_t& output) {
    output = mln::morpho::structural::opening(input, se, mln::productorder_less<uint8_t>());
  };
  this->run(st, f);
}

BENCHMARK_F(BMMorpho, Opening_By_Reconstruction_Disc)(benchmark::State& st)
{
  int  radius  = 32;
  auto se     = mln::se::disc(radius);
  auto markers = mln::morpho::structural::erode(m_input_, se);
  auto f       = [m = std::move(markers)](const image_t& input, image_t& output) {
    output = mln::morpho::opening_by_reconstruction(input, m, mln::c4);
  };
  this->run(st, f);
}


BENCHMARK_F(BMMorpho, Hit_or_miss_corner)(benchmark::State& st)
{
  mln::se::mask2d se_hit = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1},
  };

  mln::se::mask2d se_miss = {
    {1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0},
  };

  auto f = [se_hit, se_miss](const image_t& input, image_t& output) { mln::morpho::hit_or_miss(input, se_hit, se_miss, output); };
  this->run(st, f);
}

BENCHMARK_F(BMMorpho, minima)(benchmark::State& st)
{
  auto f = [](const image_t& input, image_t&) {
    int nlabel;
    mln::labeling::local_minima<int8_t>(input, mln::c4, nlabel);
    return nlabel;
  };
  this->run(st, f);
}

BENCHMARK_F(BMMorpho, watershed)(benchmark::State& st)
{
  auto f = [](const image_t& input, image_t&) {
    int nlabel;
    mln::morpho::watershed<int8_t>(input, mln::c4, nlabel);
    return nlabel;
  };
  this->run(st, f);
}


BENCHMARK_MAIN();
