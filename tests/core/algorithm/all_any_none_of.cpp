#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/any_of.hpp>
#include <mln/core/algorithm/none_of.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <gtest/gtest.h>


TEST(Core, Algorithm_Any)
{
  using namespace mln::view::ops;

  const mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};


  ASSERT_TRUE((mln::any_of(ima, [](int x) { return x == 4; })));
  ASSERT_FALSE((mln::any_of(ima, [](int x) { return x == 7; })));
  ASSERT_TRUE((mln::any_of(ima == 4)));
  ASSERT_FALSE((mln::any_of(ima == 7)));
}


TEST(Core, Algorithm_All)
{
  using namespace mln::view::ops;

  const mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};


  ASSERT_TRUE((mln::all_of(ima, [](int x) { return x > 0; })));
  ASSERT_FALSE((mln::all_of(ima, [](int x) { return x > 1; })));
  ASSERT_TRUE((mln::all_of(ima > 0)));
  ASSERT_FALSE((mln::all_of(ima > 1)));
}

TEST(Core, Algorithm_None)
{
  using namespace mln::view::ops;

  const mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};


  ASSERT_TRUE((mln::none_of(ima, [](int x) { return x > 6; })));
  ASSERT_FALSE((mln::none_of(ima, [](int x) { return x > 3; })));
  ASSERT_TRUE((mln::none_of(ima > 6)));
  ASSERT_FALSE((mln::none_of(ima > 3)));
}
