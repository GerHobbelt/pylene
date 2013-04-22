#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/labeling/rag.hpp>

#define BOOST_TEST_MODULE Labeling
#include <boost/test/unit_test.hpp>
#include <mln/io/imprint.hpp>


BOOST_AUTO_TEST_CASE(rag_test_1)
{
  using namespace mln;

  image2d<bool> ima(5,5);
  ima.at(0,0) = true;
  ima.at(0,4) = true;
  ima.at(4,0) = true;
  ima.at(4,4) = true;

  image2d<uint8> lbl;
  unsigned nlabel;
  std::tie(lbl, nlabel) = labeling::rag(ima, c4, uint8 ());

  io::imprint(lbl);
}
