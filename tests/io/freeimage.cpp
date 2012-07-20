#include <mln/core/image/image2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/io/imread.hpp>
#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>

#ifndef MLN_IMG_PATH
# error "MLN_IMG_PATH must be defined."
#endif


#define BOOST_TEST_MODULE IO
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(Freeimage_pgm)
{
  using namespace mln;

  image2d<uint8> ima;
  image2d<uint8> ref(5, 5);

  iota(ref, 1);
  io::imread(MLN_IMG_PATH "/iota2d.pgm", ima, mln::io::freeimage_reader ());

  BOOST_CHECK(equal(ima, ref));
}
