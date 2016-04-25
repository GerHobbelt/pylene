#include <array>
#include <functional>
#include <mln/core/neighborhood/dyn_wneighborhood.hpp>
#include <mln/core/image/image2d.hpp>

#define BOOST_TEST_MODULE Core
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(WNeighborhood)

template <class I>
void test_pixel_lvalue(I& f)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  dyn_wneighborhood<pset_t, wset_t, constant_neighborhood_tag> nbh(pset, wset);
  auto px = f.pixel_at({0,2});
  mln_iter(nx, nbh(px));
  {
    nx.init();
    BOOST_CHECK_EQUAL(nx->val(), 1);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(nx->weight(), -1);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 2);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(nx->weight(), 0);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 3);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,3}));
    BOOST_CHECK_EQUAL(nx->weight(), 1);
    nx.next();
    BOOST_CHECK(nx.finished());
  }
  px = f.pixel_at({1,2}); // The slidind wpixter is binded
  {
    nx.init();
    BOOST_CHECK_EQUAL(nx->val(), 6);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{1,1}));
    BOOST_CHECK_EQUAL(nx->weight(), -1);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 7);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{1,2}));
    BOOST_CHECK_EQUAL(nx->weight(), 0);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 8);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{1,3}));
    BOOST_CHECK_EQUAL(nx->weight(), 1);
    nx.next();
    BOOST_CHECK(nx.finished());
  }
}

BOOST_AUTO_TEST_CASE(binding_pixel_lvalue)
{
  mln::image2d<int> f = { {0, 1, 2, 3, 4},
                          {5, 6, 7, 8, 9} };
  test_pixel_lvalue(f);
}

BOOST_AUTO_TEST_CASE(binding_const_pixel_lvalue)
{
  const mln::image2d<int> f = { {0, 1, 2, 3, 4},
                                {5, 6, 7, 8, 9} };
  test_pixel_lvalue(f);
}


void test_point_lvalue()
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  dyn_wneighborhood<pset_t, wset_t, constant_neighborhood_tag> nbh(pset, wset);
  point2d p {0,2};
  mln_iter(n, nbh(p));
  {
    n.init();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(n->weight(), -1);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(n->weight(), 0);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,3}));
    BOOST_CHECK_EQUAL(n->weight(), 1);
    n.next();
    BOOST_CHECK(n.finished());
  }
  p = point2d{1,2}; // The slidind wpixter is binded
  {
    n.init();
    BOOST_CHECK_EQUAL(n->point(), (point2d{1,1}));
    BOOST_CHECK_EQUAL(n->weight(), -1);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{1,2}));
    BOOST_CHECK_EQUAL(n->weight(), 0);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{1,3}));
    BOOST_CHECK_EQUAL(n->weight(), 1);
    n.next();
    BOOST_CHECK(n.finished());
  }
}

BOOST_AUTO_TEST_CASE(binding_point_lvalue)
{
  test_point_lvalue();
}

template <class I>
void test_pixiterator_lvalue(I& f)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  dyn_wneighborhood<pset_t, wset_t, constant_neighborhood_tag> nbh(pset, wset);
  mln_pixter(px, f);
  mln_iter(nx, nbh(px));
  px.init();
  {
    nx.init();
    BOOST_CHECK_EQUAL(nx->val(), 0);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(nx->weight(), -1);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 0);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(nx->weight(), 0);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 1);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(nx->weight(), 1);
    nx.next();
    BOOST_CHECK(nx.finished());
  }
  px.next();
  {
    nx.init();
    BOOST_CHECK_EQUAL(nx->val(), 0);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(nx->weight(), -1);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 1);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(nx->weight(), 0);
    nx.next();
    BOOST_CHECK_EQUAL(nx->val(), 2);
    BOOST_CHECK_EQUAL(nx->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(nx->weight(), 1);
    nx.next();
    BOOST_CHECK(nx.finished());
  }
}


BOOST_AUTO_TEST_CASE(binding_pixiterator_lvalue)
{
  mln::image2d<int> f = { {0, 1, 2, 3, 4},
                          {5, 6, 7, 8, 9} };
  test_pixiterator_lvalue(f);
}

BOOST_AUTO_TEST_CASE(binding_const_pixiterator_lvalue)
{
  const mln::image2d<int> f = { {0, 1, 2, 3, 4},
                                {5, 6, 7, 8, 9} };
  test_pixiterator_lvalue(f);
}

template <class I>
void test_piterator_lvalue(I& f)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  dyn_wneighborhood<pset_t, wset_t, constant_neighborhood_tag> nbh(pset, wset);
  mln_iter(p, f.domain());
  mln_iter(n, nbh(p));
  p.init();
  {
    n.init();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(n->weight(), -1);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(n->weight(), 0);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(n->weight(), 1);
    n.next();
    BOOST_CHECK(n.finished());
  }
  p.next();
  {
    n.init();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(n->weight(), -1);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(n->weight(), 0);
    n.next();
    BOOST_CHECK_EQUAL(n->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(n->weight(), 1);
    n.next();
    BOOST_CHECK(n.finished());
  }
}


BOOST_AUTO_TEST_CASE(binding_piterator_lvalue)
{
  mln::image2d<int> f = { {0, 1, 2, 3, 4},
                          {5, 6, 7, 8, 9} };
  test_piterator_lvalue(f);
}

BOOST_AUTO_TEST_CASE(binding_const_piterator_lvalue)
{
  const mln::image2d<int> f = { {0, 1, 2, 3, 4},
                                {5, 6, 7, 8, 9} };
  test_piterator_lvalue(f);
}



BOOST_AUTO_TEST_SUITE_END()
