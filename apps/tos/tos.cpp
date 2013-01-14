#include <mln/morpho/tos/immerse.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/algorithm/copy.hpp>

#include <mln/morpho/tos/tos.hpp>
#include <mln/morpho/filtering.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <boost/format.hpp>
#include "topology.hpp"
#include <mln/morpho/maxtree_pqueue_parallel.hpp>
#include <libgen.h>
#include "thicken.hpp"

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

void usage(int argc, char** argv)
{
  if (argc < 4 or (argv[1] != std::string("mintree") && argv[1] != std::string("tos")) or
      (argv[2] != std::string("min") && argv[2] != std::string("max")))
    {
      std::cerr << "Usage: " << argv[0] << "(mintree|tos) (min|max) ima.(ppm|png|tiff...) lambda1 [lambda2  [lambda3 [...]]]" << std::endl
		<< "Compute the ToS marginally, compute area attribute and compute another ToS on it."
		<< "Output the results of filtering" << std::endl;
      abort();
    }
}


int main(int argc, char** argv)
{
  using namespace mln;

  usage(argc, argv);

  const char* filename = argv[3];

  image2d<rgb8> ima;
  io::imread(filename, ima);

  typedef UInt<9> V;
  typedef image2d<V> I;
  I r = transform(ima, [](rgb8 v) -> V { return v[0] * 2; });
  I g = transform(ima, [](rgb8 v) -> V { return v[1] * 2; });
  I b = transform(ima, [](rgb8 v) -> V { return v[2] * 2; });
  I rr = addborder(r);
  I gg = addborder(g);
  I bb = addborder(b);


  image2d<V> rK, gK, bK;
  image2d<unsigned> rparent, gparent, bparent;
  std::vector<unsigned> rS, gS, bS;

  std::tie(rK, rparent, rS) = morpho::ToS(rr, c4);
  std::tie(gK, gparent, gS) = morpho::ToS(gg, c4);
  std::tie(bK, bparent, bS) = morpho::ToS(bb, c4);

  // io::imprint(K);
  // io::imprint(parent);

  auto r_area = morpho::area_compute(rK, rparent, rS, K1::is_face_2);
  auto g_area = morpho::area_compute(gK, gparent, gS, K1::is_face_2);
  auto b_area = morpho::area_compute(bK, bparent, bS, K1::is_face_2);

  image2d<unsigned> area;
  if (std::string(argv[2]) == "max")
    area = transform(imzip(r_area, g_area, b_area), [](const boost::tuple<unsigned, unsigned, unsigned>& x) {
	return std::max(boost::get<0>(x), std::max(boost::get<1>(x), boost::get<2>(x))); });
  else
    area = transform(imzip(r_area, g_area, b_area), [](const boost::tuple<unsigned, unsigned, unsigned>& x) {
	return std::min(boost::get<0>(x), std::min(boost::get<1>(x), boost::get<2>(x))); });

  unsigned maxr = r_area[rS[0]];
  unsigned maxg = g_area[gS[0]];
  unsigned maxb = b_area[bS[0]];
  std::cout << maxr << " " << maxg << " " << maxb << std::endl;
  io::imsave(transform(r_area, [=](unsigned x) -> float { return (float)x / maxr; }), "red.tiff");
  io::imsave(transform(g_area, [=](unsigned x) -> float { return (float)x / maxg; }), "green.tiff");
  io::imsave(transform(b_area, [=](unsigned x) -> float { return (float)x / maxb; }), "blue.tiff");
  io::imsave(transform(area, [=](unsigned x) -> float { return (float)x / maxr; }), "area.tiff");
  // io::imsave(r_area, "red.tiff");
  // io::imsave(g_area, "green.tiff");
  // io::imsave(b_area, "blue.tiff");


  //++io::imprint(area);
  image2d<unsigned> K;
  image2d<unsigned> parent;
  std::vector<unsigned> S;

  bool use_tos = argv[1] == std::string("tos");

  if (use_tos)
    std::tie(K, parent, S) = morpho::ToS(area, c4);
  else
    K = area,
    std::tie(parent, S) = morpho::impl::serial::maxtree_pqueue(area, c4, std::greater<unsigned> ());

  auto w = thicken(K, parent, S);
  io::imsave(transform(w, [=](unsigned v) -> float { return (float)v; }), "thicken.tiff");
  std::exit(0);

  std::cout << "S.size(): " << S.size() << std::endl;
  auto ima2 = addborder(ima); // add border with median w.r.t < lexico
  image2d<rgb8> tmp;
  resize(tmp, parent, parent.border(), rgb8{0,0,255});

  point2d strides = use_tos ? point2d{4,4} : point2d{2,2};
  copy(ima2, tmp | sbox2d(tmp.domain().pmin, tmp.domain().pmax, strides));

  {
    image2d<unsigned> x;
    if (use_tos)
      x = morpho::area_compute(K, parent, S, K2::is_face_2);
    else
      x = morpho::area_compute(K, parent, S, K1::is_face_2, std::greater<unsigned> ());
    io::imsave(transform(x, [=](unsigned v) -> float { return (float)v; }), "area2.tiff");
  }

  for (int i = 4; i < argc; ++i) {
    int fvalue = std::atoi(argv[i]);
    image2d<rgb8> out;
    if (use_tos)
      out = morpho::area_filter(tmp, K, parent, S, fvalue, K2::is_face_2);
    else
      out = morpho::area_filter(tmp, K, parent, S, fvalue, K1::is_face_2, std::greater<unsigned> ());

    image2d<rgb8> under;
    resize(under, ima2);
    copy(out | sbox2d(out.domain().pmin, out.domain().pmax, strides), under);
    io::imsave(under, boost::str(boost::format("out-%06i.tiff") % fvalue).c_str());
  }

}
