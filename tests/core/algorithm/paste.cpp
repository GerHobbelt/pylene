#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>


#define BOOST_TEST_MODULE Algorithm
#include <boost/test/unit_test.hpp>
#include <boost/range/numeric.hpp>


BOOST_AUTO_TEST_CASE(Paste)
{
  using namespace mln;


  box2d b = {{1,1}, {3,3}};
  image2d<uint8> ima(b);
  image2d<uint8> out(15, 15);
  fill(ima, 69);
  iota(out, 1);
  paste(ima, out);
  // 1   2  3  4..
  // 16  x  x  19
  // 31  x  x  34
  // 46  47 48 49
  int r = boost::accumulate(out.values(), 0);



  BOOST_CHECK_EQUAL( r, 225*226/2 - (17+18+32+33) + 69*4 );
}
