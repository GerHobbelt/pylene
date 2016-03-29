#include <array>
#include <functional>
#include <mln/core/neighborhood/sliding_wpiter.hpp>
#include <mln/core/utils/iterator_proxy.hpp>

#define BOOST_TEST_MODULE Core
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(Sliding_wpiter)

BOOST_AUTO_TEST_CASE(binding_lvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  point2d p{0,0};
  auto it = make_sliding_wpiter(std::cref(p), pset, wset);

  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }

  p = {1,0}; // The slidind wpiter is binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }
}

BOOST_AUTO_TEST_CASE(binding_rvalue)
{
  using namespace mln;

  using pset_t = std::array<point2d, 3>;
  using wset_t = std::array<int, 3>;
  pset_t pset {{ {0,-1}, {0,0}, {0,1} }};
  wset_t wset {{ -1, 0, 1 }};

  point2d p{0,0};
  auto it = make_sliding_wpiter(p, pset, wset);

  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }

  p = {1,0}; // The sliding wpiter is not binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
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

  std::array<point2d, 2> domain{{ {0,0}, {1,0} }};
  auto p = rng::iter(domain);
  auto it = make_sliding_wpiter(make_iterator_proxy(p), pset, wset);
  p.init();
  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{0,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }

  p.next();
  // The slidind wpiter is binded
  {
    it.init();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,-1}));
    BOOST_CHECK_EQUAL(it->weight(), -1);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,0}));
    BOOST_CHECK_EQUAL(it->weight(), 0);
    it.next();
    BOOST_CHECK_EQUAL(it->point(), (point2d{1,1}));
    BOOST_CHECK_EQUAL(it->weight(), 1);
    it.next();
    BOOST_CHECK(it.finished());
  }
}

BOOST_AUTO_TEST_SUITE_END()
