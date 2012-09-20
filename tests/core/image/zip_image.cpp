#include <mln/core/image/zip_image.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>


#define BOOST_TEST_MODULE Core
#include <boost/test/unit_test.hpp>


mln::image2d<int>
make_image()
{
  mln::image2d<int> x(5, 5);
  mln::iota(x, 0);
  return x;
}

BOOST_AUTO_TEST_SUITE(ZipImage)

// Test1. Value mixing ziping
BOOST_AUTO_TEST_CASE(Mixed_writable)
{
  using namespace mln;

  image2d<int> ima(5,5);
  image2d<uint16> ima2(5,5);

  fill(imzip(ima, ima2), boost::make_tuple(2,4));
  BOOST_CHECK( all(ima == 2) );
  BOOST_CHECK( all(ima2 == 4) );
}

BOOST_AUTO_TEST_CASE(Value_Iteration_1)
{
  using namespace mln;

  image2d<int>          a(5,5);
  image2d<uint16>       b(5,5);

  auto x = imzip(a,b);
  auto values = x.values(); // This is undefined behavior to write imzip(a,b).values();
  std::for_each(std::begin(values), std::end(values),
    [](boost::tuple<int&, uint16&> w) { w = boost::make_tuple(2,4); });
  BOOST_CHECK( all(a == 2) );
  BOOST_CHECK( all(b == 4) );
}

BOOST_AUTO_TEST_CASE(Pixel_Iteration_1)
{
  using namespace mln;

  image2d<int>          a(5,5);
  image2d<uint16>       b(5,5);

  auto x = imzip(a,b);
  auto pixels = x.pixels(); // This is undefined behavior to write imzip(a,b).pixels();
  typedef zip_image<image2d<int>&, image2d<uint16>& >::pixel_type pixel_t;
  std::for_each(std::begin(pixels), std::end(pixels),
    [](pixel_t x) { x.val() = boost::make_tuple(2,4); });


  BOOST_CHECK( all(a == 2) );
  BOOST_CHECK( all(b == 4) );
}

BOOST_AUTO_TEST_CASE(Value_Iteration_2)
{
  using namespace mln;

  image2d<int>          a(5,5);
  image2d<uint16>       b(5,5);
  iota(a, 0);
  iota(b, 0);

  const auto tmp = imzip(a, b);

  int sum1 = 0;
  {
  int x, y;
  for (auto w: tmp.values())
    {
  boost::tie(x, y) = w;
  sum1 += x + y;
}
}
  BOOST_CHECK_EQUAL(sum1, 25 * 24); // sum of 25 first integers * 2
}


 BOOST_AUTO_TEST_CASE(Temporary_usage)
 {
  using namespace mln;
  image2d<int> ima(5,5);
  auto x = imzip(ima, make_image());

  for (auto w : x.values())
    boost::get<0>(w) = boost::get<1>(w);

  BOOST_CHECK( all(ima == make_image()) );
}

 BOOST_AUTO_TEST_SUITE_END()


