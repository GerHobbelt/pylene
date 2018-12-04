#include <mln/core/image/ndimage_pixel.hpp>

#include <mln/core/concept/new/pixels.hpp>

#include <type_traits>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct mock_image
{
};

template <concepts::Pixel Pix>
void foo(Pix)
{
}

TEST(Pixel, NdPixel)
{
  auto pix = mln::ndimage_pixel<int, 3, mock_image>{};
  static_assert(concepts::Pixel<decltype(pix)>);
}
