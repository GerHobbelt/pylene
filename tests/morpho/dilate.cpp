#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/morpho/dilate.hpp>
#include <mln/io/imread.hpp>


#define BOOST_TEST_MODULE Morpho
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(dilation)

using namespace mln;

BOOST_AUTO_TEST_CASE(dilate_0)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  { // Fast: border wide enough
    auto win = make_rectangle2d(7, 7);
    auto out = morpho::dilate(ima, win);
    BOOST_CHECK( all(out >= ima) ); // extensive
  }
}

// Border is not wide enough => use morpher for bound checking
BOOST_AUTO_TEST_CASE(dilate_1)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  auto win = make_rectangle2d(9, 9);
  auto out = morpho::dilate(ima, win);
  BOOST_CHECK( all(out >= ima) ); // extensive
}


// Dilation on a with a vmorph / binary case
BOOST_AUTO_TEST_CASE(dilate_2)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);


  // Border is not wide enough => use morpher for bound checking
  auto win = make_rectangle2d(3, 3);
  auto out = morpho::dilate(ima > 128, win);
  BOOST_CHECK( all(out >= (ima > 128) )); // extensive
}

// Dilation on a with a vmorph / binary case
BOOST_AUTO_TEST_CASE(dilate_3)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  auto win = make_rectangle2d(3, 3);
  auto out = clone(ima);
  auto tmp = out | where(ima > 128);
  morpho::dilate(ima | where(ima > 128), win, tmp, std::less<uint8>() );
  BOOST_CHECK( all((out | (ima <= 128)) == (ima | (ima <= 128))) );
  BOOST_CHECK( all(out >= ima) ); // extensive
}

 // Custom comparison function, erosion
BOOST_AUTO_TEST_CASE(dilate_4)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  auto win = make_rectangle2d(5, 5);
  auto out = morpho::dilate(ima, win, std::greater<uint8> ());
  BOOST_CHECK( all(out <= ima) ); // anti-extensive
}


BOOST_AUTO_TEST_SUITE_END()

