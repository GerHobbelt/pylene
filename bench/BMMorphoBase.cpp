#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/experimental/imread.hpp>

#include <mln/morpho/experimental/closing.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/hit_or_miss.hpp>
#include <mln/morpho/experimental/median_filter.hpp>
#include <mln/morpho/experimental/opening.hpp>
#include <mln/morpho/experimental/reconstruction.hpp>
#include <mln/morpho/experimental/watershed.hpp>

#include <mln/labeling/experimental/local_extrema.hpp>

#include <benchmark/benchmark.h>

#include <fixtures/ImagePath/image_path.hpp>

class BMMorpho : public benchmark::Fixture
{
public:
  using image_t = mln::experimental::image2d<uint8_t>;

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
  }


  void run(benchmark::State& st, std::function<void(const image_t& input, image_t& output)> callback)
  {
    for (auto _ : st)
      callback(m_input, m_output);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  static bool                                g_loaded;
  static mln::experimental::image2d<uint8_t> g_input;
  mln::experimental::image2d<uint8_t>        m_input;
  mln::experimental::image2d<uint8_t>        m_output;
  std::size_t                                m_size;
};

bool                                BMMorpho::g_loaded = false;
mln::experimental::image2d<uint8_t> BMMorpho::g_input;

class slow_disc : public mln::se_facade<slow_disc>
{
  using Base = mln::experimental::se::disc;
public:
  using category     = mln::dynamic_neighborhood_tag;
  using incremental  = std::false_type;
  using decomposable = std::false_type;
  using separable    = std::false_type;

  explicit slow_disc(float radius, Base::approx approximation = Base::PERIODIC_LINES_8)
    : m_disc(radius, approximation)
  {
  }

  /// \brief Return a range of SE offsets
  auto offsets() const { return m_disc.offsets(); }

  /// \brief Return a range of SE offsets before center
  auto before_offsets() const { return m_disc.before_offsets(); }

  /// \brief Return a range of SE offsets after center
  auto after_offsets() const { return m_disc.after_offsets(); }

  /// \brief Returns the extent radius
  int radial_extent() const { return m_disc.radial_extent(); }

  /// \brief Return the input ROI for 2D box.
  auto compute_input_region(mln::experimental::box2d roi) const { return m_disc.compute_input_region(roi); }

  /// \brief Return the output ROI for 2D box.
  auto compute_output_region(mln::experimental::box2d roi) const { return m_disc.compute_output_region(roi); }

private:
  Base m_disc;
};



BENCHMARK_DEFINE_F(BMMorpho, Dilation_EuclideanDisc_naive)(benchmark::State& st)
{
  int       radius = st.range(0);
  slow_disc se(radius, mln::experimental::se::disc::EXACT);

  auto f = [se](const image_t& input, image_t& output) { mln::morpho::experimental::dilation(input, se, output); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMMorpho, Dilation_EuclideanDisc_incremental)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::EXACT);

  auto f = [se](const image_t& input, image_t& output) { mln::morpho::experimental::dilation(input, se, output); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMMorpho, Dilation_ApproximatedDisc)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::PERIODIC_LINES_8);

  auto f = [se](const image_t& input, image_t& output) { mln::morpho::experimental::dilation(input, se, output); };
  this->run(st, f);
}


BENCHMARK_DEFINE_F(BMMorpho, Dilation_Square)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::rect2d(2 * radius + 1, 2 * radius + 1);
  auto f = [se](const image_t& input, image_t& output) { mln::morpho::experimental::dilation(input, se, output); };
  this->run(st, f);
}


constexpr int max_range = 128;

BENCHMARK_REGISTER_F(BMMorpho, Dilation_ApproximatedDisc)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMMorpho, Dilation_EuclideanDisc_naive)->RangeMultiplier(2)->Range(2, 16);
BENCHMARK_REGISTER_F(BMMorpho, Dilation_EuclideanDisc_incremental)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMMorpho, Dilation_Square)->RangeMultiplier(2)->Range(2, max_range);



BENCHMARK_F(BMMorpho, Opening_Disc)(benchmark::State& st)
{
  int  radius = 32;
  auto se     = mln::experimental::se::disc(radius);
  auto f = [se](const image_t& input, image_t& output) { mln::morpho::experimental::opening(input, se, output); };
  this->run(st, f);
}

BENCHMARK_F(BMMorpho, Median_Filter_Disc)(benchmark::State& st)
{
  int  radius = 32;
  auto se      = mln::experimental::se::rect2d(2 * radius + 1, 2 * radius + 1);
  auto f       = [se](const image_t& input, image_t& output) {
    mln::morpho::experimental::median_filter(input, se, mln::extension::bm::mirror{}, output);
  };
  this->run(st, f);
}


BENCHMARK_F(BMMorpho, Opening_By_Reconstruction_Disc)(benchmark::State& st)
{
  int  radius = 32;
  auto se     = mln::experimental::se::disc(radius);
  auto markers = mln::morpho::experimental::erosion(m_input, se);
  auto f       = [m = std::move(markers)](const image_t& input, image_t& output) {
    output = mln::morpho::experimental::opening_by_reconstruction(input, m, mln::experimental::c4);
  };
  this->run(st, f);
}





BENCHMARK_F(BMMorpho, Hit_or_miss_corner)(benchmark::State& st)
{
  mln::se::experimental::mask2d se_hit = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1},
  };

  mln::se::experimental::mask2d se_miss = {
    {1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 0},
  };

  auto f = [se_hit, se_miss](const image_t& input, image_t& output) { mln::morpho::experimental::hit_or_miss(input, se_hit, se_miss, output); };
  this->run(st, f);
}


BENCHMARK_F(BMMorpho, minima)(benchmark::State& st)
{
  auto f = [](const image_t& input, image_t&) {
    int nlabel;
    mln::labeling::experimental::local_minima<int8_t>(input, mln::experimental::c4, nlabel);
    return nlabel;
  };
  this->run(st, f);
}


BENCHMARK_F(BMMorpho, watershed)(benchmark::State& st)
{
  auto f = [](const image_t& input, image_t&) {
    int nlabel;
    mln::morpho::experimental::watershed<int8_t>(input, mln::experimental::c4, nlabel);
    return nlabel;
  };
  this->run(st, f);
}



BENCHMARK_MAIN();
