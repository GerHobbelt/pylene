#include <mln/core/extension/border_management.hpp>

#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <variant>
#include <gtest/gtest.h>


TEST(BorderManager, Fill_LargeEnough_BM_Auto)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);


  mln::experimental::image2d<uint8_t> ref = clone(ima);
  ref.extension().fill(42);

  auto bm                        = mln::extension::bm::fill(uint8_t(42));
  auto disc                      = mln::se::disc{1};
  auto [managed_ima, managed_se] = bm.manage(ima, disc);


  std::visit([](auto i, auto) { ASSERT_TRUE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP2(input, ref, fixtures::ImageCompare::experimental::COMPARE_EXTENSION);
      },
      managed_ima, managed_se);
}

TEST(BorderManager, Fill_NotLargeEnough_BM_Auto)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  mln::experimental::image2d<uint8_t> ref  = clone(ima);

  auto bm                        = mln::extension::bm::fill(uint8_t(42));
  auto disc                      = mln::se::disc{4};
  auto [managed_ima, managed_se] = bm.manage(ima, disc);


  std::visit([](auto i, auto) { ASSERT_FALSE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP(input, ref);
        EXPECT_EQ(input.at({-3,-3}), 42);
        EXPECT_EQ(input.at({13, 13}), 42);
      },
      managed_ima, managed_se);
}

TEST(BorderManager, Fill_LargeEnough_BM_Native)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);


  mln::experimental::image2d<uint8_t> ref = clone(ima);
  ref.extension().fill(42);

  auto bm                        = mln::extension::bm::native::fill(uint8_t(42));
  auto disc                      = mln::se::disc{1};
  auto [managed_ima, managed_se] = bm.manage(ima, disc);


  std::visit([](auto i, auto) { ASSERT_TRUE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP2(input, ref, fixtures::ImageCompare::experimental::COMPARE_EXTENSION);
      },
      managed_ima, managed_se);
}

TEST(BorderManager, Fill_NotLargeEnough_BM_Native)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);

  auto disc = mln::se::disc{4};
  auto bm   = mln::extension::bm::native::fill(uint8_t(42));
  EXPECT_THROW(bm.manage(ima, disc), std::runtime_error);
}

TEST(BorderManager, Fill_bad_cast)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);

  auto disc = mln::se::disc{1};
  auto bm   = mln::extension::bm::native::fill(42);
  EXPECT_THROW(bm.manage(ima, disc), std::runtime_error);
}
