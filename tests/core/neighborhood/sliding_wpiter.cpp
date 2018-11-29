#include <mln/core/neighborhood/sliding_wpiter.hpp>
#include <mln/core/point.hpp>
#include <mln/core/utils/wrapper.hpp>

#include <array>

#include <gtest/gtest.h>

TEST(Core, Sliding_wpiter_binding_lvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};
  wset_t wset{{-1, 0, 1}};

  point2d p{0, 0};
  auto    it = make_sliding_wpiter(std::cref(p), pset, wset);

  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }

  p = {1, 0}; // The slidind wpiter is binded
  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{1, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{1, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{1, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_wpiter_binding_rvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};
  wset_t wset{{-1, 0, 1}};

  point2d p{0, 0};
  auto    it = make_sliding_wpiter(p, pset, wset);

  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }

  p = {1, 0}; // The sliding wpiter is not binded
  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_wpiter_binding_iterator)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};
  wset_t wset{{-1, 0, 1}};

  std::array<point2d, 2> domain{{{0, 0}, {1, 0}}};
  auto                   p  = rng::iter(domain);
  auto                   it = make_sliding_wpiter(make_iterator_proxy(p), pset, wset);
  p.init();
  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }

  p.next();
  // The slidind wpiter is binded
  {
    it.init();
    ASSERT_EQ(it->point(), (point2d{1, -1}));
    ASSERT_EQ(it->weight(), -1);
    it.next();
    ASSERT_EQ(it->point(), (point2d{1, 0}));
    ASSERT_EQ(it->weight(), 0);
    it.next();
    ASSERT_EQ(it->point(), (point2d{1, 1}));
    ASSERT_EQ(it->weight(), 1);
    it.next();
    ASSERT_TRUE(it.finished());
  }
}
