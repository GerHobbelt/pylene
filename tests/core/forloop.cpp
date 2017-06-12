#include <mln/core/image/image.hpp>
#include <mln/core/domain/box.hpp>

#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(foreach_break)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  mln::point2d p;
  mln_foreach (p, box)
    if (p[0] == 2)
      break;

  BOOST_CHECK((p == mln::point2d{2,-1}));
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
