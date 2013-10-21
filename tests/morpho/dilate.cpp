#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/morpho/dilate.hpp>
#include <mln/io/imread.hpp>


#define BOOST_TEST_MODULE Morpho
#include <boost/test/unit_test.hpp>
#include <mln/io/imprint.hpp>

BOOST_AUTO_TEST_CASE(dilate_0)
{

  using namespace mln;


  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  { // Fast: border wide enough
    auto win = make_rectangle2d(7, 7);
    auto out = morpho::dilate(ima, win);
    BOOST_CHECK( all(out >= ima) ); // extensive

  }

  { // Border is not wide enough => use morpher
    auto win = make_rectangle2d(9, 9);
    auto out = morpho::dilate(ima, win);
    BOOST_CHECK( all(out >= ima) ); // extensive

  }


}


