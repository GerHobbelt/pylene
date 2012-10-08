#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/sort_offset.hpp>
#include <mln/core/grays.hpp>

#define BOOST_TEST_MODULE Algorithm
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(Sort_offset_fast)
{
  using namespace mln;


  image2d<uint8> ima(10, 10);
  iota(ima, 0);

  std::vector<std::ptrdiff_t> offset = sort_offset(ima);
  for (auto x: offset)
    std::cout << x << std::endl;
}
