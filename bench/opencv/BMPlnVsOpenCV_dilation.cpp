#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <mln/morpho/experimental/dilation.hpp>

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
      mln::experimental::image2d<mln::uint8> input(g_width, g_height);
      cv::Mat                                input_cv = cv::Mat::zeros(g_width, g_height, CV_8UC1);

      std::mt19937                    mt(42);
      std::uniform_int_distribution<> dis(0, 1);

      for (int i = 0; i < g_height; ++i)
      {
        for (int j = 0; j < g_width; ++j)
        {
          uint8_t v                  = static_cast<bool>(dis(mt)) ? 0 : 255;
          input({i, j})              = v;
          input_cv.at<uint8_t>(i, j) = v;
        }
      }

      g_input    = input;
      g_input_cv = input_cv;
      g_loaded   = true;
    }
    m_input    = g_input;
    m_input_cv = g_input_cv;
    mln::resize(m_output, m_input);
    m_size = g_width * g_height;
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
  static int                                 g_width;
  static int                                 g_height;
  static bool                                g_loaded;
  static mln::experimental::image2d<uint8_t> g_input;
  static cv::Mat                             g_input_cv;
  mln::experimental::image2d<uint8_t>        m_input;
  mln::experimental::image2d<uint8_t>        m_output;
  cv::Mat                                    m_input_cv;
  cv::Mat                                    m_output_cv;
  std::size_t                                m_size;
};
int                                 BMPlnVsOpenCV_Dilation::g_width  = 1000;
int                                 BMPlnVsOpenCV_Dilation::g_height = 1000;
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
  int  edge = st.range(0);
  auto se   = mln::experimental::se::rect2d(edge, edge);

  auto f = [se](const image_t& input, const cv_image_t&, image_t& output, cv_image_t&) {
    mln::morpho::experimental::dilation(input, se, output);
  };
  this->run(st, f);
}

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, Pln_Rect)(benchmark::State& st)
{
  int  edge = st.range(0);
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
  cv::Mat se     = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * radius, 2 * radius));

  auto f = [se](const image_t&, const cv_image_t& input_cv, image_t&, cv_image_t& output_cv) {
    cv::dilate(input_cv, output_cv, se);
  };
  this->run(st, f);
}


// CV Rect

BENCHMARK_DEFINE_F(BMPlnVsOpenCV_Dilation, CV_Rect)(benchmark::State& st)
{
  int     radius = st.range(0);
  cv::Mat se     = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * radius, 2 * radius));

  auto f = [se](const image_t&, const cv_image_t& input_cv, image_t&, cv_image_t& output_cv) {
    cv::dilate(input_cv, output_cv, se);
  };
  this->run(st, f);
}


constexpr int max_range = 128;

BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_DiscDecompPeriodic)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_DiscPeriodic)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_RectDecomp)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, Pln_Rect)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, CV_Disc)->RangeMultiplier(2)->Range(2, max_range);
BENCHMARK_REGISTER_F(BMPlnVsOpenCV_Dilation, CV_Rect)->RangeMultiplier(2)->Range(2, max_range);


BENCHMARK_MAIN();
