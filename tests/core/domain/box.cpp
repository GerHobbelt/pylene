#include <mln/core/domain/box.hpp>


#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Box)

BOOST_AUTO_TEST_CASE(box2d_general)
{
  using namespace mln;
  {
    box2d a ({0,0}, {-2,18});
    BOOST_CHECK(a.empty());
  }

  {
    short minp = - (1 << 15);
    short maxp = (1 << 15) - 1;
    unsigned n = (1<<16)-1;
    box2d a ({minp,minp}, {maxp,maxp});
    BOOST_CHECK_EQUAL(a.size(), n*n);
  }

}


BOOST_AUTO_TEST_CASE(box2d_forward)
{
  using namespace mln;

  box2d b({2,3}, {6,8});

  point2d p;
  int i = 2, j = 3;
  mln_foreach(p, b) {
    BOOST_CHECK_EQUAL(p[0], i);
    BOOST_CHECK_EQUAL(p[1], j);
    BOOST_CHECK(b.has(p));
    if (++j == 8) { j = 3; ++i; }
  }

}

BOOST_AUTO_TEST_CASE(box2d_backward)
{
  using namespace mln;

  box2d b({2,3}, {6,8});

  point2d p;
  int i = 5, j = 7;
  mln_foreach(p, b.riter()) {
    BOOST_CHECK_EQUAL(p[0], i);
    BOOST_CHECK_EQUAL(p[1], j);
    BOOST_CHECK(b.has(p));
    if (--j < 3) { j = 7; --i; }
  }
}

BOOST_AUTO_TEST_CASE(strided_box2d_forward)
{
  using namespace mln;

  sbox2d b({2,3}, {6,15}, {2,3});

  point2d p;
  int i = 2, j = 3;
  mln_foreach(p, b) {
    BOOST_CHECK_EQUAL(p[0], i);
    BOOST_CHECK_EQUAL(p[1], j);
    BOOST_CHECK(b.has(p));
    if ((j+=3) >= 15) { j = 3; i += 2; }
  }

}

BOOST_AUTO_TEST_CASE(strided_box2d_backward)
{
  using namespace mln;

  sbox2d b({2,3}, {6,17}, {2,3});

  point2d p;
  int i = 5, j = 7;
  mln_foreach(p, b.riter()) {
    BOOST_CHECK_EQUAL(p[0], i);
    BOOST_CHECK_EQUAL(p[1], j);
    BOOST_CHECK(b.has(p));
    if ((j-=3) < 3) { j = 16; i-=2; }
  }
}


BOOST_AUTO_TEST_SUITE_END()
