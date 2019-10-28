#include <mln/core/concept/new/images.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <type_traits>

#include <gtest/gtest.h>


TEST(Core, Image_Image2D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Image<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::IndexableImage<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::AccessibleImage<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::BidirectionalImage<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::RawImage<mln::experimental::image2d<int>>);
  static_assert(mln::concepts::ConcreteImage<mln::experimental::image2d<int>>);
  static_assert(not mln::concepts::ViewImage<mln::experimental::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_Image3D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Image<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::IndexableImage<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::AccessibleImage<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::BidirectionalImage<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::RawImage<mln::experimental::image3d<int>>);
  static_assert(mln::concepts::ConcreteImage<mln::experimental::image2d<int>>);
  static_assert(not mln::concepts::ViewImage<mln::experimental::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_OutputImage2D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<mln::experimental::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_OutputImage3D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<mln::experimental::image3d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
