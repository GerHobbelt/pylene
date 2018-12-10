#include <mln/core/concept/new/images.hpp>

#include <mln/core/image/image2d.hpp>
#include <mln/core/image/image3d.hpp>

#include <type_traits>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


TEST(Core, Image_Image2D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Image<mln::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_Image3D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Image<mln::image3d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
