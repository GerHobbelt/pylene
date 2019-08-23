
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/experimental/imread.hpp>

#include <benchmark/benchmark.h>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/experimental/dilation.hpp>

#include <fixtures/ImagePath/image_path.hpp>



class BMDilation : public benchmark::Fixture
{
public:
  BMDilation()
  {
    mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("lena.pgm"), m_input);
    int nr = m_input.width();
    int nc = m_input.height();
    mln::resize(m_output, m_input);
    m_size = nr * nc;
  }


  void run(benchmark::State& st, std::function<void()> callback)
  {
    for (auto _ : st)
      callback();
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  mln::experimental::image2d<uint8_t> m_input;
  mln::experimental::image2d<uint8_t> m_output;
  std::size_t    m_size;
};

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


BENCHMARK_DEFINE_F(BMDilation, EuclideanDisc_naive)(benchmark::State& st)
{
  int       radius = st.range(0);
  slow_disc se(radius, mln::experimental::se::disc::EXACT);

  auto f = [&]() { mln::morpho::experimental::dilation(m_input, se, m_output); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMDilation, EuclideanDisc_incremental)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::EXACT);

  auto f = [&]() { mln::morpho::experimental::dilation(m_input, se, m_output); };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMDilation, ApproximatedDisc)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::PERIODIC_LINES_8);

  auto f = [&]() { mln::morpho::experimental::dilation(m_input, se, m_output); };
  this->run(st, f);
}


BENCHMARK_DEFINE_F(BMDilation, Square)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::rect2d(2 * radius + 1, 2 * radius + 1);
  auto f = [&]() { mln::morpho::experimental::dilation(m_input, se, m_output); };
  this->run(st, f);
}

constexpr int max_range = 2 << 6;

BENCHMARK_REGISTER_F(BMDilation, ApproximatedDisc)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, EuclideanDisc_naive)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, EuclideanDisc_incremental)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMDilation, Square)->RangeMultiplier(2)->Range(2, max_range);


BENCHMARK_MAIN();
