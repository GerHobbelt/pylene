#include <mln/core/image/image.hpp>
#include <mln/core/domain/box.hpp>

#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(foreach_break)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  mln::point2d p = {0, 0};
  mln_foreach (p, box)
    if (p[0] == 2)
      break;

  BOOST_CHECK((p == mln::point2d{2,-1}));
}

BOOST_AUTO_TEST_CASE(foreach_0)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  mln::point2d p = {0, 0};

  int y = -1;
  int x = -1;
  mln_foreach (p, box)
  {
    BOOST_CHECK_EQUAL(p[0], y);
    BOOST_CHECK_EQUAL(p[1], x);
    if (++x == 5) { y++; x = -1; }
  }

  BOOST_CHECK_EQUAL(p, (mln::point2d{4,4}));
}

BOOST_AUTO_TEST_CASE(forall_break)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  auto p = mln::rng::iter(box);
  mln_forall (p)
    if ((*p)[0] == 2)
      break;

  BOOST_CHECK((*p == mln::point2d{2,-1}));
}

BOOST_AUTO_TEST_CASE(forall_0)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  auto p = mln::rng::iter(box);
  mln_forall (p)
  {
  }

  BOOST_CHECK((*p > mln::point2d{4,4}));
}

