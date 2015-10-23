#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/grays.hpp>
//#include <mln/io/imread.hpp>
#include <mln/io/imprint.hpp>
#include <mln/morpho/hit_or_miss.hpp>
#include <mln/core/se/utility.hpp>
#include <mln/core/image/morphers/casted_image.hpp>

#define BOOST_TEST_MODULE Morpho
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(hit_or_miss)

BOOST_AUTO_TEST_CASE(hit_or_miss)
{
  using namespace mln;

  image2d<bool> ima = {
    {0, 0, 0, 1, 1, 1},
    {0, 0, 0, 1, 1, 1},
    {0, 0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1, 1},
    {0, 0, 0, 1, 0, 1}
  };

  std::array<point2d, 2> w1 = { {{0,0}, {0,1}} };
  std::array<point2d, 1> w2 = { {{0,-1}} };

  auto win1 = se::make_se(w1);
  auto win2 = se::make_se(w2);
  auto out = morpho::hit_or_miss(ima, win1, win2);
  auto out2 = morpho::hit_or_miss(lnot(ima), win2, win1);

  BOOST_CHECK( all(out == out2) );

  auto out3 = morpho::hit_or_miss(imcast<uint8>(ima), win1, win2);
  BOOST_CHECK( all(out == out3) );
}


BOOST_AUTO_TEST_SUITE_END()
