#include <mln/core/image/image2d.hpp>
#include <mln/core/neighborhood/sliding_pixter.hpp>
#include <mln/core/utils/wrapper.hpp>

#include <array>

#include <gtest/gtest.h>

template <class I>
void
test_lvalue(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};

  auto px = f.pixel_at({0, 2});
  auto it = make_sliding_pixter(std::cref(px), pset);
  {
    it.init();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    it.next();
    ASSERT_EQ(it->val(), 3);
    ASSERT_EQ(it->point(), (point2d{0, 3}));
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px = f.pixel_at({1, 2}); // The slidind wpiter is binded
  {
    it.init();
    ASSERT_EQ(it->val(), 6);
    ASSERT_EQ(it->point(), (point2d{1, 1}));
    it.next();
    ASSERT_EQ(it->val(), 7);
    ASSERT_EQ(it->point(), (point2d{1, 2}));
    it.next();
    ASSERT_EQ(it->val(), 8);
    ASSERT_EQ(it->point(), (point2d{1, 3}));
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_pixter_binding_lvalue)
{
  mln::image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  test_lvalue(f);
}

TEST(Core, Sliding_pixter_binding_const_lvalue)
{
  const mln::image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  test_lvalue(f);
}

template <class I>
void
test_iterator(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};

  f.extension().fill(0);

  mln_pixter(px, f);
  auto it = make_sliding_pixter(make_iterator_proxy(px), pset);
  px.init();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    it.next();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    it.next();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px.next();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    it.next();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_pixter_binding_iterator)
{
  mln::image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};

  test_iterator(f);
}

TEST(Core, Sliding_pixter_binding_const_iterator)
{
  const mln::image2d<int> f = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  test_iterator(f);
}

template <class I>
void
test_IndexableButNotRaw(I& f)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};

  mln_pixter(px, f);
  auto it = make_sliding_pixter(make_iterator_proxy(px), pset);
  px.init();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, -1}));
    it.next();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px.next();
  {
    it.init();
    ASSERT_EQ(it->val(), 0);
    ASSERT_EQ(it->point(), (point2d{0, 0}));
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_EQ(it->val(), 4);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_pixter_IndexableButNotRaw)
{
  using namespace mln;
  image2d<int> f0 = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  f0.extension().fill(0);

  auto fun = [](int x) { return 2 * x; };
  auto f = imtransform(f0, fun);
  const auto const_f = imtransform(f0, fun);

  test_IndexableButNotRaw(f);
  test_IndexableButNotRaw(const_f);
}

template <class I>
void
test_AccessibleButNotIndexable(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset{{{0, -1}, {0, 0}, {0, 1}}};

  auto px = f.pixel_at({0, 2});
  auto it = make_sliding_pixter(std::cref(px), pset);
  {
    it.init();
    ASSERT_EQ(it->val(), 1);
    ASSERT_EQ(it->point(), (point2d{0, 1}));
    it.next();
    ASSERT_EQ(it->val(), 2);
    ASSERT_EQ(it->point(), (point2d{0, 2}));
    it.next();
    ASSERT_EQ(it->val(), 3);
    ASSERT_EQ(it->point(), (point2d{0, 3}));
    it.next();
    ASSERT_TRUE(it.finished());
  }

  px = f.pixel_at({1, 2}); // The slidind wpiter is binded
  {
    it.init();
    ASSERT_EQ(it->val(), 6);
    ASSERT_EQ(it->point(), (point2d{1, 1}));
    it.next();
    ASSERT_EQ(it->val(), 7);
    ASSERT_EQ(it->point(), (point2d{1, 2}));
    it.next();
    ASSERT_EQ(it->val(), 8);
    ASSERT_EQ(it->point(), (point2d{1, 3}));
    it.next();
    ASSERT_TRUE(it.finished());
  }
}

TEST(Core, Sliding_pixter_AccessibleButNotIndexable)
{
  using namespace mln;

  image2d<int> f0 = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}};
  image2d<bool> mask = {{0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}};

  auto f = f0 | where(mask);
  auto const_f = f0 | where(mask);
  test_AccessibleButNotIndexable(f);
  test_AccessibleButNotIndexable(const_f);
}
