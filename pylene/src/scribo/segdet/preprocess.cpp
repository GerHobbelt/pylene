#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/morpho/closing.hpp>
#include <mln/morpho/reconstruction.hpp>
#include <scribo/segdet.hpp>

namespace scribo::internal
{
  mln::image2d<std::uint8_t> preprocess(const mln::image2d<std::uint8_t>& input, const SegDetParams& params)
  {
    // 1. Negate

    auto neg = mln::view::transform(input, [](std::uint8_t p) -> std::uint8_t { return 255 - p; });

    // 2. Get seeds
    auto seeds = mln::view::transform(neg, [params](std::uint8_t p) -> std::uint8_t {
      return std::max(static_cast<int>(p - static_cast<int>(params.dyn * 255)), 0);
    });

    // 3. Create connectivity mask
    auto connectivity_mask = mln::morpho::dilation(neg, mln::se::rect2d{11, 11});

    // 4. Reconstruction to get the background
    auto out = mln::morpho::opening_by_reconstruction(connectivity_mask, seeds, mln::c4);
    out      = mln::transform(neg, out, [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return std::min(a, b); });

    // 5. Top hat
    mln::transform(neg, out, out, [](std::uint8_t a, std::uint8_t b) -> std::uint8_t { return 255 - (a - b); });

    return out;
  }

} // namespace mln::contrib::segdet