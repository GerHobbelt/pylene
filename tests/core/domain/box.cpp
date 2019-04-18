#include <mln/core/domain/box.hpp>

#include <mln/core/concept/new/domains.hpp>

#include <gtest/gtest.h>


#ifdef PYLENE_CONCEPT_TS_ENABLED

static_assert(mln::concepts::Domain<mln::box1d>);
static_assert(mln::concepts::Domain<mln::box1df>);
static_assert(mln::concepts::Domain<mln::box2d>);
static_assert(mln::concepts::Domain<mln::box2df>);
static_assert(mln::concepts::Domain<mln::box3d>);
static_assert(mln::concepts::Domain<mln::box3df>);

// static_assert(mln::concepts::Domain<mln::sbox1d>);
// static_assert(mln::concepts::Domain<mln::sbox2d>);
// static_assert(mln::concepts::Domain<mln::sbox3d>);

static_assert(mln::concepts::Domain<mln::grain_box1d>);
static_assert(mln::concepts::Domain<mln::grain_box1df>);
static_assert(mln::concepts::Domain<mln::grain_box2d>);
static_assert(mln::concepts::Domain<mln::grain_box2df>);
static_assert(mln::concepts::Domain<mln::grain_box3d>);
static_assert(mln::concepts::Domain<mln::grain_box3df>);

#endif // PYLENE_CONCEPT_TS_ENABLED

TEST(Core, Domain_box2d_general)
{
  using namespace mln;
  {
    box2d a({0, 0}, {0, 0});
    ASSERT_TRUE(a.empty());
  }

  {
    short    minp = -(1 << 15);
    short    maxp = (1 << 15) - 1;
    unsigned n    = (1 << 16) - 1;
    box2d    a({minp, minp}, {maxp, maxp});
    ASSERT_EQ(a.size(), n * n);
  }
}

TEST(Core, Domain_box2d_forward)
{
  using namespace mln;

  box2d b({2, 3}, {6, 8});

  point2d p;
  int     i = 2, j = 3;
  mln_foreach (p, b)
  {
    ASSERT_EQ(p[0], i);
    ASSERT_EQ(p[1], j);
    ASSERT_TRUE(b.has(p));
    if (++j == 8)
    {
      j = 3;
      ++i;
    }
  }
}

TEST(Core, Domain_box2d_forward_new)
{
  using namespace mln;

  box2d b({2, 3}, {6, 8});

  int i = 2, j = 3;
  for (point2d p : b)
  {
    ASSERT_EQ(p[0], i);
    ASSERT_EQ(p[1], j);
    ASSERT_TRUE(b.has(p));
    if (++j == 8)
    {
      j = 3;
      ++i;
    }
  }
}

TEST(Core, Domain_box2d_backward)
{
  using namespace mln;

  box2d b({2, 3}, {6, 8});

  point2d p;
  int     i = 5, j = 7;
  mln_foreach (p, b.riter())
  {
    ASSERT_EQ(p[0], i);
    ASSERT_EQ(p[1], j);
    ASSERT_TRUE(b.has(p));
    if (--j < 3)
    {
      j = 7;
      --i;
    }
  }
}

TEST(Core, Domain_box2d_backward_new)
{
  using namespace mln;

  box2d b({2, 3}, {6, 8});

  int i = 5, j = 7;
  for (point2d p : b.reversed())
  {
    ASSERT_EQ(p[0], i);
    ASSERT_EQ(p[1], j);
    ASSERT_TRUE(b.has(p));
    if (--j < 3)
    {
      j = 7;
      --i;
    }
  }
}

TEST(Core, Domain_strided_box2d_forward)
{
  using namespace mln;

  sbox2d b({2, 3}, {7, 16}, {2, 3});
  std::cout << b << std::endl;

  auto p = b.iter();
  p.init();
  for (int i = 2; i < 7; i += 2)
    for (int j = 3; j < 16; j += 3)
    {
      ASSERT_TRUE(!p.finished());
      ASSERT_EQ((*p)[0], i);
      ASSERT_EQ((*p)[1], j);
      ASSERT_TRUE(b.has(*p));
      p.next();
    }
  ASSERT_TRUE(p.finished());
}

TEST(Core, Domain_strided_box2d_backward)
{
  using namespace mln;

  sbox2d b({2, 3}, {7, 17}, {2, 3});

  auto p = b.riter();
  p.init();
  for (int i = 6; i >= 2; i -= 2)
    for (int j = 15; j >= 3; j -= 3)
    {
      ASSERT_TRUE(!p.finished());
      ASSERT_EQ((*p)[0], i);
      ASSERT_EQ((*p)[1], j);
      ASSERT_TRUE(b.has(*p));
      p.next();
    }
  ASSERT_TRUE(p.finished());
}
