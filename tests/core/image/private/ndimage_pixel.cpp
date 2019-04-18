#include <mln/core/image/private/ndimage_pixel.hpp>

#include <mln/core/concept/new/pixels.hpp>

#include <type_traits>

#include <gtest/gtest.h>


TEST(Pixel, NdPixel)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Pixel<mln::details::ndpixel<int, 3>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
