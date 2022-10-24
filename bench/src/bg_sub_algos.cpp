#include <bench/bg_sub_pln.hpp>

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
  auto            bg_blurred           = mln::morpho::gaussian2d(bg_grey, kLineHorizontalSigma, kLineVerticalSigma);

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
        auto tmp_eroded = mln::morpho::erosion(tmp_thresholded, se_);

        // Dilation (algo)
        mln::morpho::dilation(tmp_eroded, se_, output);
      },
      se);

  // std::size_t ms, rss;
  // details::GetMemorySize(ms, rss);
  // std::cout << "Amount of memory in use:" << ms << ", RSS=" << rss << std::endl;
  // details::log_memory_usage();
}
