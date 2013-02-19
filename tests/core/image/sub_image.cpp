# include <mln/core/image/image2d.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/algorithm/iota.hpp>
# include <mln/core/algorithm/fill.hpp>

# include <mln/io/imprint.hpp>

#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SubImage)

BOOST_AUTO_TEST_CASE(subdomain)
{
  using namespace mln;

  image2d<int> ima(5,5);
  iota(ima, 0);
  fill(ima | box2d{{1,1}, {4,4}}, 42);
  fill((ima | box2d{{1,1}, {4,4}}) | box2d{{1,1}, {2,3}}, 44);


  static_assert( std::is_same< decltype(ima | box2d()), image2d<int> >::value, "");
  io::imprint(ima);
}


BOOST_AUTO_TEST_CASE(mask)
{
  using namespace mln;

  image2d<int> ima(5,5);
  iota(ima, 0);
  fill(ima | (ima > 12), 12);
  fill(ima | (ima < 5) , 5);


  static_assert( std::is_same< decltype(ima | box2d()), image2d<int> >::value, "");
  io::imprint(ima);
}



BOOST_AUTO_TEST_SUITE_END()


