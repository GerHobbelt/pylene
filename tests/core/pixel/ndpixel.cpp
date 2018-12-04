#include <mln/core/image/ndimage_pixel.hpp>

#include <mln/core/concept/new/pixels.hpp>

#include <type_traits>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct mock_image
{
};

TEST(Pixel, NdPixel)
{
  [[maybe_unused]] auto pix = mln::ndimage_pixel<int, 3, mock_image>{};
  
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Pixel<decltype(pix)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

}
