#include <array>
#include <functional>
#include <mln/core/neighborhood/sliding_pixter.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/wrapper.hpp>

#define BOOST_TEST_MODULE Core
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(Sliding_pixter)

template <class I>
void test_lvalue(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};

  auto px = f.pixel_at({0,2});
  auto it = make_sliding_pixter(std::cref(px), pset);
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 3);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,3}));
    it.next();
    BOOST_CHECK(it.finished());
  }

  px = f.pixel_at({1,2}); // The slidind wpiter is binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 6);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 7);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,2}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 8);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,3}));
    it.next();
    BOOST_CHECK(it.finished());
  }

}

BOOST_AUTO_TEST_CASE(binding_lvalue)
{
  mln::image2d<int> f = { {0, 1, 2, 3, 4},
                          {5, 6, 7, 8, 9} };
  test_lvalue(f);
}

BOOST_AUTO_TEST_CASE(binding_const_lvalue)
{
  const mln::image2d<int> f = { {0, 1, 2, 3, 4},
                                {5, 6, 7, 8, 9} };
  test_lvalue(f);
}

template <class I>
void test_iterator(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};

  f.extension().fill(0);

  mln_pixter(px, f);
  auto it = make_sliding_pixter(make_iterator_proxy(px), pset);
  px.init();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK(it.finished());
  }

  px.next();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    it.next();
    BOOST_CHECK(it.finished());
  }
}

BOOST_AUTO_TEST_CASE(binding_iterator)
{
  mln::image2d<int> f = { {0, 1, 2, 3, 4},
                          {5, 6, 7, 8, 9} };

  test_iterator(f);
}

BOOST_AUTO_TEST_CASE(binding_const_iterator)
{
  const mln::image2d<int> f = { {0, 1, 2, 3, 4},
                                {5, 6, 7, 8, 9} };
  test_iterator(f);
}


template <class I>
void test_IndexableButNotRaw(I& f)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};

  mln_pixter(px, f);
  auto it = make_sliding_pixter(make_iterator_proxy(px), pset);
  px.init();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK(it.finished());
  }

  px.next();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 4);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    it.next();
    BOOST_CHECK(it.finished());
  }
}


BOOST_AUTO_TEST_CASE(IndexableButNotRaw)
{
  using namespace mln;
  image2d<int> f0 = { {0, 1, 2, 3, 4},
                      {5, 6, 7, 8, 9} };
  f0.extension().fill(0);

  auto fun = [](int x) { return 2 * x; };
  auto f = imtransform(f0, fun);
  const auto const_f = imtransform(f0, fun);

  test_IndexableButNotRaw(f);
  test_IndexableButNotRaw(const_f);
}


template <class I>
void test_AccessibleButNotIndexable(I& f)
{
  using namespace mln;
  using pset_t = std::array<point2d, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};

  auto px = f.pixel_at({0,2});
  auto it = make_sliding_pixter(std::cref(px), pset);
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 3);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,3}));
    it.next();
    BOOST_CHECK(it.finished());
  }

  px = f.pixel_at({1,2}); // The slidind wpiter is binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 6);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,1}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 7);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,2}));
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 8);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,3}));
    it.next();
    BOOST_CHECK(it.finished());
  }
}

BOOST_AUTO_TEST_CASE(AccessibleButNotIndexable)
{
  using namespace mln;


  image2d<int> f0 = { {0, 1, 2, 3, 4},
                      {5, 6, 7, 8, 9} };
  image2d<bool> mask = { {0, 0, 1, 0, 0},
                         {0, 0, 1, 0, 0} };

  auto f = f0 | where(mask);
  auto const_f = f0 | where(mask);
  test_AccessibleButNotIndexable(f);
  test_AccessibleButNotIndexable(const_f);
}



BOOST_AUTO_TEST_SUITE_END()
