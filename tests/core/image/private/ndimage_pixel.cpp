#include <mln/core/image/private/ndimage_pixel.hpp>

#include <mln/core/concept/new/pixels.hpp>

#include <type_traits>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


TEST(Pixel, NdPixel)
{
  [[maybe_unused]] auto pix = mln::details::ndpixel<int, 3>{};

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Pixel<decltype(pix)>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
