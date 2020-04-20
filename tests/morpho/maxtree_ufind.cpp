#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/value/int.hpp>
#include <mln/io/imprint.hpp>
#include <mln/morpho/maxtree_ufind.hpp>

#include <random>

#include <gtest/gtest.h>

TEST(Morpho, maxtree_ufind)
{
  using namespace mln;

  image2d<uint8> ima(5, 5);
  iota(ima, 2);

  io::imprint(ima);
  {
    image2d<std::size_t> parent = morpho::maxtree(ima, c4, std::less<uint8>());
    io::imprint(parent);
  }

  {
    image2d<std::size_t> parent = morpho::maxtree(ima, c4, std::greater<uint8>());
    io::imprint(parent);
  }
}
