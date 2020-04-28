#include <mln/core/extension/border_management.hpp>


#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>

#include <variant>
#include <gtest/gtest.h>


TEST(Core, Mirror_LargeEnough_BM_Auto)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  mln::experimental::image2d<uint8_t> ref = clone(ima);
  ref.extension().mirror();

  auto disc                      = mln::experimental::se::disc{1};
  auto [managed_ima, managed_se] = mln::extension::bm::mirror().manage(ima, disc);

  std::visit([](auto i, auto) { ASSERT_TRUE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP2(input, ref, fixtures::ImageCompare::experimental::COMPARE_EXTENSION);
      },
      managed_ima, managed_se);
}

TEST(Core, Mirror_NotLargeEnough_BM_Auto)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  mln::experimental::image2d<uint8_t> ref = clone(ima);

  auto disc                      = mln::experimental::se::disc{4};
  auto [managed_ima, managed_se] = mln::extension::bm::mirror().manage(ima, disc);

  std::visit([](auto i, auto) { ASSERT_FALSE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP(input, ref);
        EXPECT_EQ(input.at({-3,-3}), 22);
        EXPECT_EQ(input.at({12, 12}), 77);
      },
      managed_ima, managed_se);
}

TEST(Core, Mirror_LargeEnough_BM_Native)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);
  mln::experimental::image2d<uint8_t> ref = clone(ima);
  ref.extension().mirror();

  auto disc                      = mln::experimental::se::disc{1};
  auto [managed_ima, managed_se] = mln::extension::bm::native::mirror().manage(ima, disc);

  std::visit([](auto i, auto) { ASSERT_TRUE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP2(input, ref, fixtures::ImageCompare::experimental::COMPARE_EXTENSION);
      },
      managed_ima, managed_se);
}

TEST(Core, Mirror_NotLargeEnough_BM_Native)
{
  mln::experimental::image2d<uint8_t> ima(10, 10);

  auto disc = mln::experimental::se::disc{4};
  auto bm   = mln::extension::bm::native::mirror();
  EXPECT_THROW(bm.manage(ima, disc), std::runtime_error);
}

// TODO: implement mirror with padding in ndimage and test-it
