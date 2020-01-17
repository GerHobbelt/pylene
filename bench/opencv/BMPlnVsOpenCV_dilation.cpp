#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/experimental/imread.hpp>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <mln/morpho/experimental/dilation.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <benchmark/benchmark.h>
#include <opencv2/opencv.hpp>

#include <random>


class BMPlnVsOpenCV_Dilation : public benchmark::Fixture
{
public:
  using image_t    = mln::experimental::image2d<uint8_t>;
  using cv_image_t = cv::Mat;

  BMPlnVsOpenCV_Dilation()
  {
    if (!g_loaded)
    {
      const char*                           filename = "Aerial_view_of_Olbia.jpg";
      mln::experimental::image2d<mln::rgb8> input;
      mln::io::experimental::imread(filename, input);

      g_input          = mln::transform(input, [](mln::rgb8 x) -> uint8_t { return x[0]; });
      cv::Mat input_cv = cv::Mat::zeros(g_input.width(), g_input.height(), CV_8UC1);

      for (int i = 0; i < g_input.height(); ++i)
      {
        for (int j = 0; j < g_input.width(); ++j)
        {
          input_cv.at<uint8_t>(j, i) = g_input({j, i});
        }
      }

      g_input_cv = input_cv;
      g_loaded   = true;
    }
    m_input    = g_input;
    m_input_cv = g_input_cv;
    mln::resize(m_output, m_input);
    m_size = g_input.width() * g_input.height();
  }

  void run(benchmark::State& st,
           std::function<void(const image_t& input, const cv_image_t& input_cv, image_t& output, cv_image_t& output_cv)>
               callback)
  {
    for (auto _ : st)
      callback(m_input, m_input_cv, m_output, m_output_cv);
    st.SetBytesProcessed(int64_t(st.iterations()) * int64_t(m_size));
  }

protected:
  static bool                                g_loaded;
  static mln::experimental::image2d<uint8_t> g_input;
  static cv::Mat                             g_input_cv;
  mln::experimental::image2d<uint8_t>        m_input;
  mln::experimental::image2d<uint8_t>        m_output;
  cv::Mat                                    m_input_cv;
  cv::Mat                                    m_output_cv;
  std::size_t                                m_size;
};
bool                                BMPlnVsOpenCV_Dilation::g_loaded = false;
mln::experimental::image2d<uint8_t> BMPlnVsOpenCV_Dilation::g_input;
cv::Mat                             BMPlnVsOpenCV_Dilation::g_input_cv;


// PLN Disc

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_DiscDecompPeriodic)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::PERIODIC_LINES_8);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_DiscPeriodic)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc_non_decomp(radius, mln::experimental::se::disc::PERIODIC_LINES_8);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_DiscDecompIncremental)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc(radius, mln::experimental::se::disc::EXACT);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_DiscIncremental)(benchmark::State& st)
{
  int  radius = st.range(0);
  auto se     = mln::experimental::se::disc_non_decomp(radius, mln::experimental::se::disc::EXACT);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}


// PLN rect

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_RectDecomp)(benchmark::State& st)
{
  int  edge = 2 * st.range(0) + 1;
  auto se   = mln::experimental::se::rect2d(edge, edge);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_Rect)(benchmark::State& st)
{
  int  edge = 2 * st.range(0) + 1;
  auto se   = mln::experimental::se::rect2d_non_decomp(edge, edge);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}


// CV Disc

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, CV_Disc)(benchmark::State& st)
{
  int     radius = st.range(0);
  cv::Mat se     = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * radius + 1, 2 * radius + 1));

  auto f = [se](const image_t&, const cv_image_t& input_cv, image_t&, cv_image_t& output_cv) {
    cv::dilate(input_cv, output_cv, se);
  };
  this->run(st, f);
}


// CV Rect

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, CV_Rect)(benchmark::State& st)
{
  int     edge = 2 * st.range(0) + 1;
  cv::Mat se   = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(edge, edge));

  auto f = [se](const image_t&, const cv_image_t& input_cv, image_t&, cv_image_t& output_cv) {
    cv::dilate(input_cv, output_cv, se);
  };
  this->run(st, f);
}


constexpr int max_range = 128;

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_DiscDecompPeriodic)->RangeMultiplier(2)->Range(1, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_DiscPeriodic)->RangeMultiplier(2)->Range(1, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_RectDecomp)->RangeMultiplier(2)->Range(1, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_Rect)->RangeMultiplier(2)->Range(1, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, CV_Disc)->RangeMultiplier(2)->Range(1, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, CV_Rect)->RangeMultiplier(2)->Range(1, max_range);


BENCHMARK_MAIN();
