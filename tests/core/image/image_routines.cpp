#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imprint.hpp>

#include <gtest/gtest.h>

TEST(Core, Image2d_Where_binary)
{
  using namespace mln;

  image2d<int> ima(5, 5);

  iota(ima, 0);
  auto d = where(ima < 10);

  mln_foreach (const point2d& p, d)
  {
    ASSERT_TRUE(d.has(p));
    ASSERT_TRUE(ima(p) < 10);
  }

  mln_foreach (const auto& px, ima.pixels())
  {
    if (px.val() >= 10)
      ASSERT_TRUE(not d.has(px.point()));
    else
      ASSERT_TRUE(d.has(px.point()));
  }
}

TEST(Core, Image2d_Where_vfunction)
{
  using namespace mln;

  image2d<int> ima(5, 5);

  iota(ima, 0);
  auto d = where(ima, [](long x) { return x < 10; });

  mln_foreach (const point2d& p, d)
  {
    ASSERT_TRUE(d.has(p));
    ASSERT_TRUE(ima(p) < 10);
  }

  mln_foreach (const auto& px, ima.pixels())
  {
    if (px.val() >= 10)
      ASSERT_TRUE(not d.has(px.point()));
    else
      ASSERT_TRUE(d.has(px.point()));
  }
}

TEST(Core, Image2d_Where_pixfunction)
{
  using namespace mln;

  typedef image2d<int> I;
  image2d<int>         ima(5, 5);

  iota(ima, 0);
  auto d = where(ima, [](const mln_pixel(const I) & x) { return x.val() < 10; });

  mln_foreach (const point2d& p, d)
  {
    ASSERT_TRUE(d.has(p));
    ASSERT_TRUE(ima(p) < 10);
  }

  mln_foreach (const auto& px, ima.pixels())
  {
    if (px.val() >= 10)
      ASSERT_TRUE(not d.has(px.point()));
    else
      ASSERT_TRUE(d.has(px.point()));
  }
}
