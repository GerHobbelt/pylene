#include <mln/core/image/view/cast.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <gtest/gtest.h>


TEST(View, cast)
{
  using namespace mln::view::ops;

  mln::image2d<double> ima = {{0.1, 1.2, 2.3, 3.4, 4.5}, //
                              {5.6, 6.7, 4.8, 8.7, 9.6}, //
                              {10.5, 11.4, 12.3, 13.2, 14.1}};

  mln::image2d<int> ref = {{0, 1, 2, 3, 4}, //
                           {5, 6, 4, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  auto casted_ima = mln::view::cast<int>(ima);
  ASSERT_TRUE(all_of(casted_ima == ref));

  mln::image2d<double> ima2 = {{0.1, 0.2, 0.3, 0.4, 0.5}, //
                               {0.6, 0.7, 0.8, 0.7, 0.6}, //
                               {0.5, 0.4, 0.3, 0.2, 0.1}};

  ASSERT_TRUE(all_of(ima == casted_ima + ima2));
}
