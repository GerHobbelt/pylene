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
  auto            bg_blurred           = mln::morpho::gaussian2d(bg_grey, kLineHorizontalSigma, kLineVerticalSigma);

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
        auto tmp_eroded = mln::morpho::erosion(tmp_thresholded, se_);

        // Dilation (algo)
        mln::morpho::dilation(tmp_eroded, se_, output);
      },
      se);
}
