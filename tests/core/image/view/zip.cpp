#include <mln/core/image/view/zip.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <mln/core/range/algorithm/for_each.hpp>
#include <mln/core/range/foreach.hpp>

#include <gtest/gtest.h>

mln::image2d<int> make_image()
{
  mln::image2d<int> x(5, 5);
  mln::iota(x, 0);
  return x;
}

template <mln::concepts::Image I>
void foo(I&);

// Test1. Value mixing ziping
TEST(Core, ZipImage_Mixed_writable)
{
  using namespace mln::view::ops;

  mln::image2d<int>    ima(5, 5);
  mln::image2d<uint16_t> ima2(5, 5);

  mln::iota(ima, 0);
  mln::iota(ima2, 1);

  auto x = mln::view::zip(ima, ima2);
  static_assert(not mln::concepts::ConcreteImage<decltype(x)>);
  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableImage<decltype(x)>);
  static_assert(mln::concepts::AccessibleImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  //static_assert(mln::concepts::BidirectionalImage<decltype(x)>); // FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);

  fill(x, std::make_tuple(2, 4));

  ASSERT_TRUE(mln::all_of(ima == 2));
  ASSERT_TRUE(mln::all_of(ima2 == 4));
}


TEST(Core, ZipImage_Value_Iteration_1)
{
  using namespace mln::view::ops;

  mln::image2d<int>    a(5, 5);
  mln::image2d<uint16_t> b(5, 5);

  auto x = mln::view::zip(a, b);


  static_assert(not mln::concepts::ConcreteImage<decltype(x)>);
  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableImage<decltype(x)>);
  static_assert(mln::concepts::AccessibleImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  //static_assert(mln::concepts::BidirectionalImage<decltype(x)>); // FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);

  mln::ranges::for_each(x.values(), [](std::tuple<int&, uint16_t&> w) { w = std::make_tuple(2, 4); });

  ASSERT_TRUE(mln::all_of(a == 2));
  ASSERT_TRUE(mln::all_of(b == 4));
}

TEST(Core, ZipImage_Pixel_Iteration_1)
{
  using namespace mln::view::ops;

  mln::image2d<int>      a(5, 5);
  mln::image2d<uint16_t> b(5, 5);

  auto x = mln::view::zip(a, b);
  static_assert(not mln::concepts::ConcreteImage<decltype(x)>);
  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableImage<decltype(x)>);
  static_assert(mln::concepts::AccessibleImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  // static_assert(mln::concepts::BidirectionalImage<decltype(x)>); // FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);


  mln::ranges::for_each(x.pixels(), [](auto px) { px.val() = std::make_tuple(2, 4); });

  ASSERT_TRUE(mln::all_of(a == 2));
  ASSERT_TRUE(mln::all_of(b == 4));
}

TEST(Core, ZipImage_Value_Iteration_2)
{
  mln::image2d<int>      a(5, 5);
  mln::image2d<uint16_t> b(5, 5);
  mln::iota(a, 0);
  mln::iota(b, 0);

  auto x = mln::view::zip(a, b);
  static_assert(not mln::concepts::ConcreteImage<decltype(x)>);
  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableImage<decltype(x)>);
  static_assert(mln::concepts::AccessibleImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  // static_assert(mln::concepts::BidirectionalImage<decltype(x)>); // FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);

  int sum1 = 0;
  {
    mln_foreach ((auto [x, y]), x.values())
      sum1 += x + y;
  }
  ASSERT_EQ(sum1, 25 * 24); // sum of 25 first integers * 2
}

TEST(Core, ZipImage_Temporary_usage)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima(5, 5);
  auto                            x = mln::view::zip(ima, make_image());

  static_assert(not mln::concepts::ConcreteImage<decltype(x)>);
  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableImage<decltype(x)>);
  static_assert(mln::concepts::AccessibleImage<decltype(x)>);
  static_assert(not mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  // static_assert(mln::concepts::BidirectionalImage<decltype(x)>); //FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);

  mln_foreach (auto w, x.values())
    std::get<0>(w) = std::get<1>(w);

  ASSERT_TRUE(mln::all_of(ima == make_image()));
}
