#include <mln/core/image/image2d.hpp>


#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(Image2d)

BOOST_AUTO_TEST_CASE(construction)
{
  using namespace mln;

  image2d<int>          a;       // default construction
  image2d<char>		b(5,5);  // construction from nrows, ncols
  image2d<int>		c(box2d{{5,5}, {10,10}}); // construction from domain

  image2d<int>		d = c;   // Copy construction
  const image2d<int>&	e = a;
  image2d<const int>	f = a;   // Copy construction
  image2d<const int>	g = e;   // Copy construction

  // Should work soons
  //image2d<int>		e = b;   // Assignment
}



BOOST_AUTO_TEST_SUITE_END()
