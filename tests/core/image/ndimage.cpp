#include <mln/core/concepts/image.hpp>

#include <mln/core/image/ndimage.hpp>
#include <type_traits>

#include <gtest/gtest.h>


TEST(Core, Image_Image2D)
{
  static_assert(mln::concepts::Image<mln::image2d<int>>);
  static_assert(mln::concepts::IndexableImage<mln::image2d<int>>);
  static_assert(mln::concepts::AccessibleImage<mln::image2d<int>>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::image2d<int>>);
  static_assert(mln::concepts::BidirectionalImage<mln::image2d<int>>);
  static_assert(mln::concepts::RawImage<mln::image2d<int>>);
  static_assert(mln::concepts::ConcreteImage<mln::image2d<int>>);
  static_assert(not mln::concepts::ViewImage<mln::image2d<int>>);
}


TEST(Core, Image_Image3D)
{
  static_assert(mln::concepts::Image<mln::image3d<int>>);
  static_assert(mln::concepts::IndexableImage<mln::image3d<int>>);
  static_assert(mln::concepts::AccessibleImage<mln::image3d<int>>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<mln::image3d<int>>);
  static_assert(mln::concepts::BidirectionalImage<mln::image3d<int>>);
  static_assert(mln::concepts::RawImage<mln::image3d<int>>);
  static_assert(mln::concepts::ConcreteImage<mln::image2d<int>>);
  static_assert(not mln::concepts::ViewImage<mln::image2d<int>>);
}


TEST(Core, Image_OutputImage2D)
{
  static_assert(mln::concepts::OutputImage<mln::image2d<int>>);
}


TEST(Core, Image_OutputImage3D)
{
  static_assert(mln::concepts::OutputImage<mln::image3d<int>>);
}
