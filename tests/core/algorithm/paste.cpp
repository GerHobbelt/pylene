#include <mln/core/algorithm/paste.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/range/algorithm/accumulate.hpp>

#include <range/v3/numeric/accumulate.hpp>

#include <gtest/gtest.h>


TEST(Core, Experimental_Algorithm_Paste)
{
  mln::experimental::box2d            b({1, 1}, {3, 3});
  mln::experimental::image2d<uint8_t> ima(b);
  mln::experimental::image2d<uint8_t> out(15, 15);
  mln::fill(ima, 69);
  mln::iota(out, 1);
  mln::paste(ima, out);
  // 1   2  3  4..
  // 16  x  x  19
  // 31  x  x  34
  // 46  47 48 49
  int r = ::ranges::accumulate(out.new_values(), 0);

  ASSERT_EQ(r, 225 * 226 / 2 - (17 + 18 + 32 + 33) + 69 * 4);
}
