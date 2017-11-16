#include <mln/core/image/image2d.hpp>
#include <mln/core/neighborhood/sliding_wpixter.hpp>
#include <mln/core/point.hpp>
#include <mln/core/utils/wrapper.hpp>

#include <array>

#include <gtest/gtest.h>

TEST(Core, Sliding_wpixter_binding_lvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};
  wset_t wset{{-1, 0, 1}};

  image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};

  auto px = f.pixel_at({0, 2});
  auto it = make_sliding_wpixter(std::cref(px), pset, wset);
  {
    it.init();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->val(), 3);
    ASSERT_EQ(it->point(), (point2d{0, 3}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px = f.pixel_at({1, 2}); // The slidind wpiter is binded
  {
    it.init();
    ASSERT_EQ(it->val(), 6);
    ASSERT_EQ(it->point(), (point2d{1, 1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->val(), 7);
    ASSERT_EQ(it->point(), (point2d{1, 2}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->val(), 8);
    ASSERT_EQ(it->point(), (point2d{1, 3}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_wpixter_binding_iterator)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};
  wset_t wset{{-1, 0, 1}};

  image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  f.extension().fill(0);

  mln_pixter(px, f);
  auto it = make_sliding_wpixter(make_iterator_proxy(px), pset, wset);
  px.init();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px.next();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }
}
