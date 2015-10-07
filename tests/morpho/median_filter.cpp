#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/morpho/median_filter.hpp>
#include <mln/io/imread.hpp>
#include <vector>
#include <algorithm>


#define BOOST_TEST_MODULE Morpho
#include <tests/test.hpp>

BOOST_AUTO_TEST_SUITE(median_filter)

using namespace mln;


image2d<uint8>
naive_median(const image2d<uint8>& f, rect2d win, int sz)
{
  mln_entering("naive_median");

  image2d<uint8> g;
  resize(g,f);

  mln_pixter(px, qx, f, g);
  mln_iter(nx, win(px));

  std::vector<uint8> V;

  mln_forall(px, qx)
  {
    V.clear();
    mln_forall(nx)
      V.push_back(nx->val());
    std::sort(V.begin(), V.end());
    qx->val() = V[sz / 2];
  }

  mln_exiting();
  return g;
}


BOOST_AUTO_TEST_CASE(median_filter_0)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "lena.pgm", ima);

  { // Fast: border wide enough
    auto win = make_rectangle2d(7, 7);
    auto out = morpho::median_filter(ima, win);
    auto out2 = naive_median(ima, win, 49);
    MLN_CHECK_IMEQUAL(out2, out);
  }
}


BOOST_AUTO_TEST_SUITE_END()