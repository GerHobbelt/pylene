#include <mln/contrib/segdet/detect_line.hpp>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/closing.hpp>
#include <mln/morpho/reconstruction.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>


namespace mln::contrib::segdet
{
  mln::image2d<uint8_t> preprocess_img_grayscale(mln::image2d<uint8_t> input)
  {
    // 1. Negate
    input = mln::transform(input, [](uint8_t p) -> uint8_t { return 255 - p; });

    // 2. Get seeds
    auto seeds = mln::transform(
        input, [](uint8_t p) -> uint8_t { return std::max(static_cast<int>(p - static_cast<int>(0.6 * 255)), 0); });

    // 3. Create connectivity mask
    auto connectivity_mask = mln::morpho::dilation(input, mln::se::rect2d{11, 11});

    // 4. Reconstruction to get the background
    auto out = mln::morpho::opening_by_reconstruction(connectivity_mask, seeds, mln::c4);
    out      = mln::transform(input, out, [](uint8_t a, uint8_t b) -> uint8_t { return std::min(a, b); });

    // 5. Top hat
    mln::transform(input, out, out, [](uint8_t a, uint8_t b) -> uint8_t { return 255 - (a - b); });

    return out;
  }

  mln::image2d<uint8_t> preprocess_img_rgb(mln::image2d<mln::rgb8> img)
  {
    // Convert image to greyscale
    mln::image2d<uint8_t> input = mln::transform(std::move(img), [](mln::rgb8 p) -> uint8_t {
      double r = p[0];
      double g = p[1];
      double b = p[2];
      return 0.2125 * r + 0.7154 * g + 0.0721 * b;
    });

    return preprocess_img_grayscale(input);
  }

  mln::ndbuffer_image preprocess(mln::ndbuffer_image img)
  {
    mln::image2d<uint8_t> out;

    switch (img.sample_type())
    {
    case mln::sample_type_id::UINT8:
      out = mln::contrib::segdet::preprocess_img_grayscale(img.__cast<uint8_t, 2>());
      break;
    case mln::sample_type_id::RGB8:
      out = mln::contrib::segdet::preprocess_img_rgb(img.__cast<mln::rgb8, 2>());
      break;
    default:;
    }
    return std::move(out);
  }
} // namespace mln::contrib::segdet