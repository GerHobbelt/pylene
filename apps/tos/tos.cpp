#include <mln/morpho/tos/immerse.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/algorithm/copy.hpp>

#include <mln/morpho/tos/tos.hpp>
#include <mln/morpho/filtering.hpp>

namespace mln
{

  template < class I, class Compare = std::less<mln_value(I)> >
  mln_concrete(I)
  addborder(const Image<I>& ima_, const Compare& cmp = Compare ())
  {
    const I& ima = exact(ima_);
    typedef mln_value(I) V;
    image2d<V> out(ima.nrows() + 2, ima.ncols() + 2);

    {
      box2d box = ima.domain();
      box.pmin += 1; box.pmax += 1;
      copy(ima, out | box);
    }

    V median;
    unsigned ncols = ima.ncols(), nrows = ima.nrows();
    {
      std::vector<V> border;
      border.reserve(2 * (nrows + ncols) - 4);
      for (int i = 0; i < ncols; ++i) {
	border.push_back(ima.at(0,i));
	border.push_back(ima.at(nrows-1,i));
      }

      for (int i = 1; i < nrows-1; ++i) {
	border.push_back(ima.at(i,0));
	border.push_back(ima.at(i,ncols-1));
      }

      std::cout << border.size() << std::endl;
      std::partial_sort(border.begin(), border.begin() + border.size()/2+1, border.end(), cmp);
      if (border.size() % 2 == 0) {
	V a = border[border.size()/2 - 1], b = border[border.size()/2];
	median = a + (b-a) / 2;
      } else
	median = border[border.size()/2];
    }

    {
      for (int i = 0; i < ncols+2; ++i) {
	out.at(0,i) = median;
	out.at(nrows+1,i) = median;
      }

      for (int i = 1; i < nrows+1; ++i) {
	out.at(i,0) = median;
	out.at(i,ncols+1) = median;
      }
    }
    return out;
  }

}


int main()
{
  using namespace mln;

  image2d<uint8> ima(4,4);
  iota(ima, 1);
  auto w = transform(ima, [](uint8 v) { return (short)(2*v); });
  auto z = addborder(w);


  image2d<short> K;
  image2d<unsigned> parent;
  std::vector<unsigned> S;

  std::tie(K, parent, S) = morpho::ToS(z, c4);
  io::imprint(K);
  io::imprint(parent);

  auto area = morpho::area_compute(K, parent, S);
  io::imprint(area);


  for (o,ss
}
