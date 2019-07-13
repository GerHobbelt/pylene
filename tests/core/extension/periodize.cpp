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


TEST(Core, Periodize_LargeEnough_BM_Auto)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out  = clone(ima);
  [[maybe_unused]] auto           disc = mln::se::disc{1};

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
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  image2d<uint8> out             = clone(ima);
  auto           disc            = mln::se::disc{4};
  auto [managed_ima, managed_se] = extension::bm::periodize().manage(ima, disc);

  ASSERT_FALSE(
      std::visit([](auto i, auto) { return mln::extension::is_finite(i.extension()); }, managed_ima, managed_se));
  ASSERT_TRUE(std::visit(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima, managed_se));
  std::visit([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_NE_EXP(i, ima); }, managed_ima, managed_se);

  // TODO: implement periodize in ndimage
  // ima.extension().periodize();
  // std::visit([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima, managed_se);
}

TEST(Core, Periodize_LargeEnough_BM_Native)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out  = clone(ima);
  [[maybe_unused]] auto           disc = mln::se::disc{1};

  // TODO: implement periodize in ndimage
  /*
  auto [managed_ima, managed_se] = extension::bm::native::periodize().manage(ima, disc);
  ASSERT_TRUE(std::visit([](auto i, auto) { rreturn mln::extension::is_finite(i.extension()); },
                                           managed_ima, managed_se));
  ASSERT_TRUE(std::visit(
      [&out](auto i, auto) {
        using namespace mln::view::ops;
        return all_of(i == out);
      },
      managed_ima, managed_se));
  std::visit([&ima](auto i, auto) { ASSERT_IMAGES_WITH_BORDER_EQ_EXP(i, ima); }, managed_ima, managed_se);
  */
}

TEST(Core, Periodize_NotLargeEnough_BM_Native)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<uint8> ima(10, 10);
  iota(ima, 0);
  [[maybe_unused]] image2d<uint8> out = clone(ima);

  [[maybe_unused]] auto disc = mln::se::disc{4};
  [[maybe_unused]] auto bm   = extension::bm::native::periodize();
  // TODO: implement periodize in ndimage
  // EXPECT_THROW(bm.manage(ima, disc), std::runtime_error);
}
