#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>

#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <range/v3/algorithm/for_each.hpp>

#include <gtest/gtest.h>

mln::image2d<int> make_image()
{
  mln::image2d<int> x(5, 5);
  mln::iota(x, 0);
  return x;
}

// Test1. Value mixing ziping
TEST(Core, ZipImage_Mixed_writable)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  image2d<uint16> ima2(5, 5);

  iota(ima, 0);
  iota(ima2, 1);

  fill(view::zip(ima, ima2), std::make_tuple(2, 4));
  ASSERT_TRUE(new_all(new_eq(ima, 2)));
  ASSERT_TRUE(new_all(new_eq(ima2, 4)));
}


TEST(Core, ZipImage_Value_Iteration_1)
{
  using namespace mln;

  image2d<int> a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);
  ::ranges::for_each(x.new_values(), [](std::tuple<int&, uint16&> w) { w = std::make_tuple(2, 4); });

  ASSERT_TRUE(new_all(new_eq(a, 2)));
  ASSERT_TRUE(new_all(new_eq(b, 4)));
}

TEST(Core, ZipImage_Pixel_Iteration_1)
{
  using namespace mln;

  image2d<int> a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);
  ::ranges::for_each(x.new_pixels(), [](auto px) { px.val() = std::make_tuple(2, 4); });

  ASSERT_TRUE(new_all(new_eq(a, 2)));
  ASSERT_TRUE(new_all(new_eq(b, 4)));
}

TEST(Core, ZipImage_Value_Iteration_2)
{
  using namespace mln;

  image2d<int> a(5, 5);
  image2d<uint16> b(5, 5);
  iota(a, 0);
  iota(b, 0);

  auto tmp = view::zip(a, b);

  int sum1 = 0;
  {
    mln_foreach_new ((auto [x, y]), tmp.new_values())
      sum1 += x + y;
  }
  ASSERT_EQ(sum1, 25 * 24); // sum of 25 first integers * 2
}

TEST(Core, ZipImage_Temporary_usage)
{
  using namespace mln;

  image2d<int> ima(5, 5);
  auto x = view::zip(ima, make_image());

  mln_foreach_new (auto w, x.new_values())
    std::get<0>(w) = std::get<1>(w);

  ASSERT_TRUE(new_all(new_eq(ima, make_image())));
}

