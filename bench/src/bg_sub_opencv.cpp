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
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/erosion.hpp>
#include <mln/morpho/gaussian2d.hpp>

#include <opencv2/opencv.hpp>

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
}
