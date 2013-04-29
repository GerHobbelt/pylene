#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/extension/fill.hpp>

#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(_image2d)


BOOST_AUTO_TEST_CASE(extension)
{
  using namespace mln;

  image2d<int> ima(5,5);
  image2d<int> ref(5,5);


  iota(ima, 1);
  iota(ref, 1);

  mln::extension::fill(ima, 69);

  mln_pixter(p1, p2, ima, ref);
  mln_iter(q1, c4(p1));
  mln_iter(q2, c4(p2));

  mln_forall(p1, p2)
    mln_forall(q1, q2)
    {
      if (ima.domain().has(q1->point()))
        BOOST_CHECK_EQUAL(q1->val(), q2->val());
      else
        BOOST_CHECK_EQUAL(q1->val(), 69);
    }
}



BOOST_AUTO_TEST_SUITE_END()
