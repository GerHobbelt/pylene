#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/extension/border_management.hpp>
#include <mln/core/extension/fill.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imprint.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <variant>

#include <gtest/gtest.h>


TEST(Core, Extension_Fill)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out = clone(ima);

  extension::fill(ima, 42);

  ASSERT_TRUE(all_of(ima == out));
}


TEST(Core, Fill_LargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out = clone(ima);

  auto disc        = mln::se::disc{1};
  auto managed_ima = extension::bm::fill(uint8_t(42)).manage(ima, disc);

  ASSERT_TRUE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima);
}

TEST(Core, Fill_NotLargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out         = clone(ima);
  auto           disc        = mln::se::disc{4};
  auto           managed_ima = extension::bm::fill(uint8_t(42)).manage(ima, disc);

  ASSERT_FALSE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_NE_EXP(i, ima); }, managed_ima);
  ima.extension().fill(uint8_t(42));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima);
}

TEST(Core, Fill_LargeEnough_BM_Native)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out = clone(ima);

  auto disc        = mln::se::disc{1};
  auto managed_ima = extension::bm::native::fill(uint8_t(42)).manage(ima, disc);

  ASSERT_TRUE(mln::extension::visit_result([](auto i, auto) { return i.extension().is_finite(); }, managed_ima));
  ASSERT_TRUE(mln::extension::visit_result(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima));
  mln::extension::visit_result([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima);
}

TEST(Core, Fill_NotLargeEnough_BM_Native)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out = clone(ima);

  auto disc = mln::se::disc{4};
  auto bm   = extension::bm::native::fill(uint8_t(42));
  EXPECT_THROW(auto managed_ima = bm.manage(ima, disc), std::runtime_error);
}
