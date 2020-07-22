#include <bench/bg_sub.hpp>

#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/views.hpp>
#include <mln/core/range/view/transform.hpp>
#include <mln/core/range/view/zip.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/gaussian2d.hpp>

#include <opencv2/opencv.hpp>

void pln_bg_sub_pipe_views(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, const pln_se_t& se)
{
  // GrayScale (view)
  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

  auto img_grey = mln::view::transform(img_color, grayscale);
  auto bg_grey  = mln::view::transform(bg_color, grayscale);

  // Gaussian on BG (algo)
  constexpr float kLineVerticalSigma   = 5;
  constexpr float kLineHorizontalSigma = 5;
  auto bg_blurred = mln::morpho::gaussian2d(bg_grey,  kLineHorizontalSigma, kLineVerticalSigma);

  // Substract (view)
  using namespace mln::view::ops;
  auto tmp_grey = img_grey - bg_blurred;

  // Thresholding (view)
  const float threshold       = 150;
  auto        thesholding_fun = [threshold](auto x) -> uint8_t { return (x < threshold) ? 0 : 255; };
  auto        tmp_thresholded = mln::view::transform(tmp_grey, thesholding_fun);


  std::visit(
      [&](auto&& se_) {
        // Erosion (algo)
        auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, se_);

        // Dilation (algo)
        mln::morpho::experimental::dilation(tmp_eroded, se_, output);
      },
      se);

  // std::size_t ms, rss;
  // details::GetMemorySize(ms, rss);
  // std::cout << "Amount of memory in use:" << ms << ", RSS=" << rss << std::endl;
  // details::log_memory_usage();
}


void pln_bg_sub_pipe_algos(const mln::image2d<mln::rgb8>& img_color, const mln::image2d<mln::rgb8>& bg_color,
                           mln::image2d<uint8_t>& output, const pln_se_t& se)
{
  // GrayScale (algo)
  mln::image2d<uint8_t> img_grey, bg_grey;
  mln::resize(img_grey, img_color);
  mln::resize(bg_grey, bg_color);
  {
    auto zipped_inputs  = mln::view::zip(img_color, bg_color);
    auto zipped_outputs = mln::view::zip(img_grey, bg_grey);
    mln::transform(zipped_inputs, zipped_outputs, [](auto&& input_vals) {
      auto&& [img_val, bg_val] = input_vals;
      return std::make_tuple(0.2126 * img_val[0] + 0.7152 * img_val[1] + 0.0722 * img_val[2],
                             0.2126 * bg_val[0] + 0.7152 * bg_val[1] + 0.0722 * bg_val[2]);
    });
  }


  // Gaussian on BG (algo)
  constexpr float kLineVerticalSigma   = 5;
  constexpr float kLineHorizontalSigma = 5;
  auto bg_blurred = mln::morpho::gaussian2d(bg_grey,  kLineHorizontalSigma, kLineVerticalSigma);

  // Substract (algo)
  mln::image2d<uint8_t> tmp_grey;
  mln::resize(tmp_grey, img_grey);
  {
    auto zipped_inputs = mln::view::zip(img_grey, bg_grey);
    mln::transform(zipped_inputs, tmp_grey, [](auto&& input_vals) {
      auto&& [img_grey_v, bg_grey_v] = input_vals;
      return img_grey_v - bg_grey_v;
    });
  }

  // thresholding (algo)
  constexpr float       threshold = 150;
  mln::image2d<uint8_t> tmp_thresholded;
  mln::resize(tmp_thresholded, tmp_grey);
  mln::transform(tmp_grey, tmp_thresholded,
                 [threshold](auto&& input_val) { return (input_val < threshold) ? 0 : 255; });

  std::visit(
      [&](auto&& se_) {
        // Erosion (algo)
        auto tmp_eroded = mln::morpho::experimental::erosion(tmp_thresholded, se_);

        // Dilation (algo)
        mln::morpho::experimental::dilation(tmp_eroded, se_, output);
      },
      se);

  // std::size_t ms, rss;
  // details::GetMemorySize(ms, rss);
  // std::cout << "Amount of memory in use:" << ms << ", RSS=" << rss << std::endl;
  // details::log_memory_usage();
}

void cv_bg_sub_pipe(const cv::Mat& img_color, const cv::Mat& bg_color, cv::Mat& output, const cv::Mat& se)
{
  (void)img_color;
  (void)bg_color;
  (void)output;

  // GrayScale (algo)
  cv::Mat img_grey, bg_grey;
  {
    mln_entering("opencv::grayscale");

    cv::cvtColor(img_color, img_grey, cv::COLOR_RGB2GRAY);
    cv::cvtColor(bg_color, bg_grey, cv::COLOR_RGB2GRAY);
  }

  // Gaussian on BG (algo)

  constexpr float kLineVerticalSigma   = 5;
  constexpr float kLineHorizontalSigma = 5;
  cv::Mat         bg_blurred;
  {
    mln_entering("opencv::gaussianblur");
    cv::GaussianBlur(bg_grey, bg_blurred, cv::Size(0, 0), kLineVerticalSigma, kLineHorizontalSigma);
  }

  // Substract (algo)
  auto                           tmp_grey = cv::Mat(img_color.rows, img_color.cols, CV_8UC1);
  cv::MatConstIterator_<uint8_t> it_img = img_grey.begin<uint8_t>(), it_img_end = img_grey.end<uint8_t>();
  cv::MatConstIterator_<uint8_t> it_bg  = bg_blurred.begin<uint8_t>();
  cv::MatIterator_<uint8_t>      it_out = tmp_grey.begin<uint8_t>();
  {
    mln_entering("opencv::substract");
    for (; it_img != it_img_end; ++it_img, ++it_bg, ++it_out)
    {
      *it_out = *it_img - *it_bg;
    }
  }

  // thresholding (algo)
  cv::Mat tmp_thresholded;
  {
    mln_entering("opencv::threshold");
    cv::threshold(tmp_grey, tmp_thresholded, 150, 255, cv::THRESH_BINARY);
  }

  // erosion (algo)

  auto tmp_eroded = cv::Mat(img_color.rows, img_color.cols, CV_8UC1);
  {
    mln_entering("opencv::erode");
    cv::erode(tmp_thresholded, tmp_eroded, se);
  }

  // dilation (algo)
  {
    mln_entering("opencv::dilate");
    cv::dilate(tmp_eroded, output, se);
  }

  // std::size_t ms, rss;
  // details::GetMemorySize(ms, rss);
  // std::cout << "Amount of memory in use:" << ms << ", RSS=" << rss << std::endl;
  // details::log_memory_usage();
}
