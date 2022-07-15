#include "../detect_line.hpp"
#include <scribo/segdet.hpp>


#include <stdexcept>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/closing.hpp>
#include <mln/morpho/reconstruction.hpp>

namespace scribo::internal
{
  mln::image2d<std::uint8_t> black_top_hat(const mln::image2d<std::uint8_t>& input, const Descriptor& descriptor)
  {
    // 1. Negate
    auto neg = mln::view::transform(input, [](std::uint8_t p) -> std::uint8_t { return 255 - p; });

    // 2. Get seeds
    auto seeds = mln::view::transform(neg, [descriptor](std::uint8_t p) -> std::uint8_t {
      return std::max(static_cast<int>(p - static_cast<int>(descriptor.dyn * 255)), 0);
    });

    // 3. Create connectivity mask
    auto connectivity_mask = mln::morpho::dilation(neg, mln::se::rect2d{descriptor.size_mask, descriptor.size_mask});

    // 4. Reconstruction to get the background
    auto out = mln::morpho::opening_by_reconstruction(connectivity_mask, seeds, mln::c4);
    out      = mln::transform(neg, out, [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return std::min(a, b); });

    // 5. Top hat
    mln::transform(neg, out, out, [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return 255 - (a - b); });

    return out;
  }

  mln::image2d<std::uint8_t> preprocess(const mln::image2d<std::uint8_t>& input, const Descriptor& descriptor)
  {
    switch (descriptor.preprocess)
    {
    case SEGDET_PREPROCESS_ENUM::NONE:
      return input;
    case SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT:
      return black_top_hat(input, descriptor);
    default:
      throw std::runtime_error("Bad preprocess choice. Possible are NONE(0), BLACK_TOP_HAT(1)");
    }
  }

} // namespace scribo::internal