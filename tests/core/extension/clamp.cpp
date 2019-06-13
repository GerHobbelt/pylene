#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imprint.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <variant>

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

  auto managed_ima = extension::bm::clamp().manage(ima, disc);

  /*
  ASSERT_TRUE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima);
  */
}

TEST(Core, Clamp_NotLargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out         = clone(ima);
  auto           disc        = mln::se::disc{4};
  auto           managed_ima = extension::bm::clamp().manage(ima, disc);

  ASSERT_FALSE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_NE_EXP(i, ima); }, managed_ima);

  // TODO: implement clamp in ndimage
  // ima.extension().periodize();
  // mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); },
  // managed_ima);
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

  auto managed_ima = extension::bm::native::clamp().manage(ima, disc);
  /*
  ASSERT_TRUE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima);
  */
}

TEST(Core, Clamp_NotLargeEnough_BM_Native)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out = clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{4};
  [[maybe_unused]] auto bm   = extension::bm::native::clamp();
  // TODO: implement clamp in ndimage
  // EXPECT_THROW(auto managed_ima = bm.manage(ima, disc), std::runtime_error);
}
