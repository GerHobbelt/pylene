#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imprint.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


TEST(Core, Clamp_LargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out = clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{1};
  // TODO: implement clamp in ndimage
  /*
  auto extended_ima = extension::bm::clamp().manage(ima, disc);


  ASSERT_TRUE(extended_ima.extension().is_finite());
  ASSERT_TRUE(all_of(extended_ima == out));
  ASSERT_IMAGES_WITH_BORDER_EQ_EXP(extended_ima, ima);
  */
}

TEST(Core, Clamp_NotLargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out          = clone(ima);
  auto           disc         = mln::se::disc{4};
  auto           extended_ima = extension::bm::clamp().manage(ima, disc);

  ASSERT_FALSE(extended_ima.extension().is_finite());
  ASSERT_TRUE(all_of(extended_ima == out));
  ASSERT_IMAGES_WITH_BORDER_NE_EXP(extended_ima, ima);

  // TODO: implement clamp in ndimage
  // ima.extension().clamp();
  // ASSERT_IMAGES_WITH_BORDER_EQ_EXP(extended_ima, ima);
}

TEST(Core, Clamp_LargeEnough_BM_User)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out = clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{1};
  // TODO: implement clamp in ndimage
  /*
  auto extended_ima = extension::bm::user_managed::clamp().manage(ima, disc);

  ASSERT_TRUE(extended_ima.extension().is_finite());
  ASSERT_TRUE(all_of(extended_ima == out));
  ASSERT_IMAGES_WITH_BORDER_EQ_EXP(extended_ima, ima);
  */
}

TEST(Core, Clamp_NotLargeEnough_BM_User)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out = clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{4};
  [[maybe_unused]] auto bm   = extension::bm::user_managed::clamp();
  // TODO: implement clamp in ndimage
  // EXPECT_THROW(auto extended_ima = bm.manage(ima, disc), std::runtime_error);
}
