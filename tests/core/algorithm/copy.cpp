#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>

#include <mln/core/algorithm/copy.hpp>


#define BOOST_TEST_MODULE Algorithm
#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm_ext/iota.hpp>


BOOST_AUTO_TEST_CASE(Copy)
{
  using namespace mln;


  image2d<uint8> ima(10, 10);
  image2d<uint8> out(10, 10);
  boost::iota(ima.values(), 0);
  copy(ima, out);
  BOOST_CHECK( boost::equal(ima.values(), out.values()) );
}
