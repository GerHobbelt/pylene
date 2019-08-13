#include <mln/core/extension/border_management.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/disc.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <variant>

#include <gtest/gtest.h>


TEST(Core, Periodize_LargeEnough_BM_Auto)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);
  [[maybe_unused]] mln::experimental::image2d<uint8_t> out  = mln::clone(ima);
  [[maybe_unused]] auto                                disc = mln::se::disc{1};

  // TODO: implement periodize in ndimage
  /*
  auto [managed_ima, managed_se] = extension::bm::periodize().manage(ima, disc);
  ASSERT_TRUE(
      std::visit([](auto i, auto) { return mln::extension::is_finite(i.extension()); }, managed_ima, managed_se));
  ASSERT_TRUE(std::visit(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima, managed_se));
  std::visit([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima, managed_se);
  */
}

TEST(Core, Periodize_NotLargeEnough_BM_Auto)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);

  mln::experimental::image2d<uint8_t> ref = mln::clone(ima);

  auto disc                      = mln::se::disc{4};
  auto [managed_ima, managed_se] = mln::extension::bm::periodize().manage(ima, disc);

  std::visit([](auto i, auto) { ASSERT_FALSE(mln::extension::is_finite(i.extension())); }, managed_ima, managed_se);
  std::visit(
      [&ref](auto input, auto) {
        ASSERT_IMAGES_EQ_EXP(input, ref);
        // To be implemented
        // EXPECT_EQ(input.at({-3,-3}), input.at(7,7));
        // EXPECT_EQ(input.at({12, 12}), input.at(3,3));
      },
      managed_ima, managed_se);
}

TEST(Core, Periodize_LargeEnough_BM_Native)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);
  [[maybe_unused]] mln::experimental::image2d<uint8_t> out  = mln::clone(ima);
  [[maybe_unused]] auto           disc = mln::se::disc{1};

  // TODO: implement periodize in ndimage
  /*
  auto [managed_ima, managed_se] = mln::extension::bm::native::periodize().manage(ima, disc);
  ASSERT_TRUE(std::visit([](auto i, auto) { rreturn mln::extension::is_finite(i.extension()); },
                                           managed_ima, managed_se));
  ASSERT_TRUE(std::visit(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima, managed_se));
  std::visit([&ima](auto i, auto) { ASSERT_IMAGES_EQ_EXP2(i, ima, fixtures::ImageCompare::experimental::COMPARE_EXTENSION); }, managed_ima, managed_se);
  */
}

TEST(Core, Periodize_NotLargeEnough_BM_Native)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima(10, 10);
  mln::iota(ima, 0);
  [[maybe_unused]] mln::experimental::image2d<uint8_t> out = mln::clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{4};
  [[maybe_unused]] auto bm   = mln::extension::bm::native::periodize();
  // TODO: implement periodize in ndimage
  // EXPECT_THROW(bm.manage(ima, disc), std::runtime_error);
}
