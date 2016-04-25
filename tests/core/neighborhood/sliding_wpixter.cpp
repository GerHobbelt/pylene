#include <array>
#include <functional>
#include <mln/core/neighborhood/sliding_wpixter.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/utils/iterator_proxy.hpp>

#define BOOST_TEST_MODULE Core
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(Sliding_wpixter)

BOOST_AUTO_TEST_CASE(binding_lvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  image2d<int> f = { {0, 1, 2, 3, 4},
                     {5, 6, 7, 8, 9} };

  auto px = f.pixel_at({0,2});
  auto it = make_sliding_wpixter(std::cref(px), pset, wset);
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 3);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,3}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }

  px = f.pixel_at({1,2}); // The slidind wpiter is binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 6);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 7);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,2}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 8);
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,3}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }
}

BOOST_AUTO_TEST_CASE(binding_iterator)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  image2d<int> f = { {0, 1, 2, 3, 4},
                     {5, 6, 7, 8, 9} };
  f.extension().fill(0);

  mln_pixter(px, f);
  auto it = make_sliding_wpixter(make_iterator_proxy(px), pset, wset);
  px.init();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }

  px.next();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->val(), 0);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 1);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->val(), 2);
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,2}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }
}



BOOST_AUTO_TEST_SUITE_END()
