#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/range/algorithm/for_each.hpp>

#include <range/v3/algorithm/for_each.hpp>

#include <boost/tuple/tuple_io.hpp>

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
  using namespace mln::experimental::ops;

  image2d<int>    ima(5, 5);
  image2d<uint16> ima2(5, 5);
  mln::iota(ima, 0);
  mln::iota(ima2, 1);

  // auto x = imzip(ima, ima2);
  // mln_viter(v, x);
  // mln_forall(v)
  //   std::cout << std::get<0>(*v) << "," <<  std::get<1>(*v)    << std::endl;
  auto z = view::zip(ima, ima2);
  mln::fill(z, std::make_tuple(2, 4));
  ASSERT_TRUE(all_of(ima == 2));
  ASSERT_TRUE(all_of(ima2 == 4));
}

TEST(Core, ZipImage_Value_Iteration_1)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);
  ::ranges::for_each(x.new_values(), [](auto w) { w = std::make_tuple(2, 4); });
  ASSERT_TRUE(all_of(a == 2));
  ASSERT_TRUE(all_of(b == 4));
}

TEST(Core, ZipImage_Pixel_Iteration_1)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);
  ::ranges::for_each(x.new_pixels(), [](auto x) { x.val() = std::make_tuple(2, 4); });

  ASSERT_TRUE(all_of(a == 2));
  ASSERT_TRUE(all_of(b == 4));
}

TEST(Core, ZipImage_Value_Iteration_2)
{
  using namespace mln;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);
  mln::iota(a, 0);
  mln::iota(b, 0);

  auto tmp = view::zip(a, b);

  int sum1 = 0;
  {
    for (auto&& [x, y] : tmp.new_values())
    {
      sum1 += x + y;
    }
  }
  ASSERT_EQ(sum1, 25 * 24); // sum of 25 first integers * 2
}

TEST(Core, ZipImage_Temporary_usage)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int> ima(5, 5);
  auto         tmp = make_image();
  auto         z   = view::zip(ima, tmp);

  for (auto&& [x, y] : z.new_values())
  {
    x = y;
  }

  ASSERT_TRUE(all_of(ima == make_image()));
}
