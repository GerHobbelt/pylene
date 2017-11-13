#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/core/se/utility.hpp>
#include <mln/core/win2d.hpp>
#include <mln/io/imprint.hpp>
#include <mln/morpho/hit_or_miss.hpp>

#include <gtest/gtest.h>

TEST(Morpho, hit_or_miss)
{
  using namespace mln;

  image2d<bool> ima = {
      {0, 0, 0, 1, 1, 1}, {0, 0, 0, 1, 1, 1}, {0, 0, 0, 1, 1, 1}, {0, 0, 1, 1, 1, 1}, {0, 0, 0, 1, 0, 1}};

  std::array<point2d, 2> w1 = {{{0, 0}, {0, 1}}};
  std::array<point2d, 1> w2 = {{{0, -1}}};

  auto win1 = se::make_se(w1);
  auto win2 = se::make_se(w2);
  auto out = morpho::hit_or_miss(ima, win1, win2);
  auto out2 = morpho::hit_or_miss(lnot(ima), win2, win1);

  ASSERT_TRUE(all(out == out2));

  auto out3 = morpho::hit_or_miss(imcast<uint8>(ima), win1, win2);
  ASSERT_TRUE(all(imcast<uint8>(out) == out3));
}
