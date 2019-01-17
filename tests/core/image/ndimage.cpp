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
  static_assert(concepts::IndexableImage<mln::image2d<int>>);
  static_assert(concepts::AccessibleImage<mln::image2d<int>>);
  static_assert(concepts::IndexableAndAccessibleImage<mln::image2d<int>>);
  static_assert(concepts::BidirectionalImage<mln::image2d<int>>);
  static_assert(concepts::RawImage<mln::image2d<int>>);
  static_assert(concepts::ConcreteImage<mln::image2d<int>>);
  static_assert(not concepts::ViewImage<mln::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_Image3D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Image<mln::image3d<int>>);
  static_assert(concepts::IndexableImage<mln::image3d<int>>);
  static_assert(concepts::AccessibleImage<mln::image3d<int>>);
  static_assert(concepts::IndexableAndAccessibleImage<mln::image3d<int>>);
  static_assert(concepts::BidirectionalImage<mln::image3d<int>>);
  static_assert(concepts::RawImage<mln::image3d<int>>);
  static_assert(concepts::ConcreteImage<mln::image2d<int>>);
  static_assert(not concepts::ViewImage<mln::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_OutputImage2D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::OutputImage<mln::image2d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, Image_OutputImage3D)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::OutputImage<mln::image3d<int>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}