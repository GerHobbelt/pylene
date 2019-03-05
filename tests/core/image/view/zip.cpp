#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>

#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/zip.hpp>
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
  using namespace mln::experimental::ops;

  image2d<int>    ima(5, 5);
  image2d<uint16> ima2(5, 5);

  mln::iota(ima, 0);
  mln::iota(ima2, 1);

  auto x = view::zip(ima, ima2);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(not concepts::ConcreteImage<decltype(x)>);
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(not concepts::IndexableImage<decltype(x)>);
  static_assert(concepts::AccessibleImage<decltype(x)>);
  static_assert(not concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  fill(x, std::make_tuple(2, 4));

  ASSERT_TRUE(mln::all_of(ima == 2));
  ASSERT_TRUE(mln::all_of(ima2 == 4));
}


TEST(Core, ZipImage_Value_Iteration_1)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(not concepts::ConcreteImage<decltype(x)>);
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(not concepts::IndexableImage<decltype(x)>);
  static_assert(concepts::AccessibleImage<decltype(x)>);
  static_assert(not concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  ::ranges::for_each(x.new_values(), [](std::tuple<int&, uint16&> w) { w = std::make_tuple(2, 4); });

  ASSERT_TRUE(mln::all_of(a == 2));
  ASSERT_TRUE(mln::all_of(b == 4));
}

TEST(Core, ZipImage_Pixel_Iteration_1)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);

  auto x = view::zip(a, b);
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(not concepts::ConcreteImage<decltype(x)>);
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(not concepts::IndexableImage<decltype(x)>);
  static_assert(concepts::AccessibleImage<decltype(x)>);
  static_assert(not concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  ::ranges::for_each(x.new_pixels(), [](auto px) { px.val() = std::make_tuple(2, 4); });

  ASSERT_TRUE(mln::all_of(a == 2));
  ASSERT_TRUE(mln::all_of(b == 4));
}

TEST(Core, ZipImage_Value_Iteration_2)
{
  using namespace mln;

  image2d<int>    a(5, 5);
  image2d<uint16> b(5, 5);
  mln::iota(a, 0);
  mln::iota(b, 0);

  auto x = view::zip(a, b);
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(not concepts::ConcreteImage<decltype(x)>);
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(not concepts::IndexableImage<decltype(x)>);
  static_assert(concepts::AccessibleImage<decltype(x)>);
  static_assert(not concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  int sum1 = 0;
  {
    mln_foreach_new ((auto [x, y]), x.new_values())
      sum1 += x + y;
  }
  ASSERT_EQ(sum1, 25 * 24); // sum of 25 first integers * 2
}

TEST(Core, ZipImage_Temporary_usage)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  image2d<int> ima(5, 5);
  auto         x = view::zip(ima, make_image());
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(not concepts::ConcreteImage<decltype(x)>);
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(not concepts::IndexableImage<decltype(x)>);
  static_assert(concepts::AccessibleImage<decltype(x)>);
  static_assert(not concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  mln_foreach_new (auto w, x.new_values())
    std::get<0>(w) = std::get<1>(w);

  ASSERT_TRUE(mln::all_of(ima == make_image()));
}
