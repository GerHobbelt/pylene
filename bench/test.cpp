#include <mln/core/image/image2d.hpp>
//#include <mln/core/neighb2d.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <mln/core/rangev3/view/zip.hpp>

using R = mln::details::ndpix_range<uint16_t, 2>;

void my_test(R a, R b)
{
  auto z = mln::ranges::view::zip(a, b);
  for (auto r : z.rows())
    for (auto [pxIn, pxOut] : r)
    {
      int sum = 0;
      for (auto nx : mln::c8_new(pxIn))
        sum += nx.val();
      pxOut.val() = sum;
      //pxOut.val() = 2 * pxIn.val();
    }
}
