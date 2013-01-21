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
#include <mln/morpho/maxtree_ufind_parallel.hpp>
#include <libgen.h>
#include "thicken.hpp"
#include "addborder.hpp"
#include "compute_attribute.hpp"


void usage(int argc, char** argv)
{
  if (argc < 5 or (argv[1] != std::string("mintree") && argv[1] != std::string("tos")) or
      (argv[2] != std::string("min") && argv[2] != std::string("max")))
    {
      std::cerr << "Usage: " << argv[0] << "(mintree|tos) (min|max) ima.(ppm|png|tiff...) mu1 [lambda2  [lambda3 [...]]]" << std::endl
		<< "Compute the ToS marginally -> area -> a Tree and run simplification."
		<< "Output the results of filtering" << std::endl;
      abort();
    }
}

namespace mln
{

  template <typename V, typename T, class FilterFun>
  image2d<V>
  setmean_on_nodes(const image2d<V>& ima, const image2d<T>& K,
		   const image2d<unsigned>& parent, const std::vector<unsigned>& S, FilterFun pred)
  {
    mln_precondition(ima.domain() == K.domain());
    mln_precondition(parent.domain() == K.domain());

    typedef rgb<unsigned> SumType;
    image2d<SumType>  sum;
    image2d<unsigned> count;
    resize(count, K, K.border(), 0);
    resize(sum, K, K.border(), SumType ());

    for (int i = S.size() - 1; i >= 0; --i)
      {
	unsigned p = S[i];
	if (pred(K.point_at_index(p)))
	  {
	    unsigned q = parent[p];
	    if (K[p] != K[q])
	      q = p;
	    count[q] += 1;
	    sum[q] += ima[p];
	  }
      }

    image2d<V> out;
    resize(out, ima, ima.border(), literal::zero);
    unsigned p = S[0];
    out[p] = sum[p] / count[p];
    for (unsigned p: S)
      {
	unsigned q = parent[p];
        assert(K[q] != K[parent[q]] or q == parent[q]);

	if (K[p] == K[q] or count[p] == 0) {
	  out[p] = out[q];
        } else {
	  out[p] = sum[p] / count[p];
	}
        assert(out[p] != literal::zero);
      }
    return out;
  }



  struct attr_2f
  {
    attr_2f()
      : size (0),
	sum(literal::zero),
	sum2(literal::zero)
    {
    }


    template <typename V>
    void take(const V& x)
    {
      ++size;
      vec3f y = x.as_vec();
      sum += y;
      sum2 += y * y;
    }


    float to_result()
    {
      return (sum2 - (sum * sum) / size).sum();
    }

    int   size;
    vec3f sum;
    vec3f sum2;
  };



  struct attr_1f
  {
    attr_1f()
      : size (0),
	gradient (0)
    {
    }

    void take(float grad_)
    {
      gradient = std::max(gradient, grad_);
      ++size;
    }

    int size;
    float gradient;
  };


  template <typename V, typename T, class FilterFun>
  void
  set_mean_on_01face(const image2d<T>& K, const image2d<unsigned>& parent,
		     const std::vector<unsigned>& S, image2d<V>& out, FilterFun filter)
  {
    struct myattribute {
      unsigned count;
      vec3i sum;
    };

    mln_precondition(K.domain() == out.domain());
    mln_precondition(K.domain() == parent.domain());

    image2d<myattribute> attr;
    resize(attr, K, K.border(), myattribute ());

    for (int i = S.size() - 1; i >= 0; --i)
      {
	unsigned p = S[i];
	point2d realp = out.point_at_index(p);
	if (filter(realp))
	  {
	    unsigned q = K[p] == K [parent[p]]  ? parent[p] : p;
	    ++attr[q].count;
	    attr[q].sum += out[p].as_vec();
	  }
      }

    mln_foreach(point2d p, out.domain())
      {
	if (not filter(p))
	  {
	    unsigned i = out.index_of_point(p);
	    unsigned q = K[i] == K[parent[i]] ? parent[i] : i;
	    myattribute a = attr[q];
	    while (a.count == 0) {
	      q = parent[q];
	      a = attr[q];
	    }
	    out[i] = (V) (a.sum / a.count);
	  }
      }
  }

  // p must be node (caonical element)
  inline
  unsigned
  findcanonical(const image2d<bool>& is_removed, image2d<unsigned>& parent, unsigned p)
  {
    if (!is_removed[p])
      return p;
    else
      return parent[p] = findcanonical(is_removed, parent, parent[p]);
  }


  template <typename T>
  void
  simplify(image2d<T>& K, image2d<unsigned>& parent, const std::vector<unsigned>& S,
	   image2d<attr_2f>& acc2, image2d<attr_1f>& acc1, float mu)
  {
    std::vector<unsigned> nodes;

    nodes.reserve(S.size());
    //nodes.push_back(S[0]); all execept the root
    for (unsigned p : S)
      if (K[parent[p]] != K[p])
	nodes.push_back(p);
    std::sort(nodes.begin(), nodes.end(), [&] (unsigned x, unsigned y) { return acc1[x].gradient < acc1[y].gradient; });

    unsigned nnodes = nodes.size();
    std::cout << "Number of nodes: " << nnodes << std::endl;

    image2d<bool> is_removed;
    resize(is_removed, K, K.border(), false);

    bool need_repeat = true;
    auto sqr = [] (vec3f x) { return x * x; };
    unsigned cpt = 0;
    while (need_repeat)
      {
	need_repeat = false;
        unsigned i = 0;
	for (unsigned p: nodes) {
	  if (!is_removed[p])
	    {
              unsigned q = findcanonical(is_removed, parent, parent[p]);
	      float delta_e = (-sqr(acc2[p].sum) / acc2[p].size - sqr(acc2[q].sum) / acc2[q].size +
                               sqr(acc2[p].sum + acc2[q].sum) / (acc2[p].size + acc2[q].size)).sum();
              //std::cout << (-sqr(acc2[p].sum) / acc2[p].size).sum() << std::endl;
              //std::cout << (-sqr(acc2[q].sum) / acc2[q].size).sum() << std::endl;
              //std::cout << (-sqr(acc2[q].sum + acc2[p].sum) / (acc2[p].size + acc2[q].size)).sum() << std::endl;
              if (std::isfinite(delta_e) and delta_e > (-mu * acc1[p].size) )
                {
                  ++cpt;
                  need_repeat = true;
                  is_removed[p] = true;
                  acc2[q].sum += acc2[p].sum;
                  acc2[q].sum2 += acc2[p].sum2;
                  acc2[q].size += acc2[p].size;
                  parent[p] = q;
                }
	    }
          ++i;
        }
      }

    std::cout << "remove: " << cpt << " nodes." << std::endl;

    // recanonize
    for (int i = S.size() - 1; i >= 0; --i)
      {
        unsigned p = S[i];
        unsigned q = parent[p];
        assert(K[q] != K[parent[q]] or q == parent[q]);
        if (K[p] == K[q]) // p non canonical
          {
            q = findcanonical(is_removed, parent, q);
            K[p] = K[q];
            parent[p] = q;
          }
        else // p canonical
          {
            q = findcanonical(is_removed, parent, p);
            if (p != q) {
              K[p] = K[q];
              parent[p] = q;
            } else {
              parent[p] = findcanonical(is_removed, parent, parent[p]);
            }
          }
      }

    nnodes = 1;
    for (unsigned p: S)
      {
        unsigned q = parent[p];
        assert(!is_removed[q]);
        assert(K[q] != K[parent[q]] or q == parent[q]);
        nnodes += (K[p] != K[q]);
      }


    std::cout << "Number of nodes: " << nnodes << std::endl;
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

  auto r_area = morpho::area_compute(rK, rparent, rS);//, K1::is_face_2);
  auto g_area = morpho::area_compute(gK, gparent, gS);//, K1::is_face_2);
  auto b_area = morpho::area_compute(bK, bparent, bS);//, K1::is_face_2);

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
  io::imsave(transform(area, [=](unsigned x) -> float { return (float)x; }), "area.tiff");
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
    std::tie(parent, S) = morpho::impl::serial::maxtree_ufind(area, c8, std::greater<unsigned> ());

  auto ima2 = addborder(ima); // add border with median w.r.t < lexico
  image2d<rgb8> tmp;
  resize(tmp, parent, parent.border(), rgb8{0,0,255});

  point2d strides = use_tos ? point2d{4,4} : point2d{2,2};
  copy(ima2, tmp | sbox2d(tmp.domain().pmin, tmp.domain().pmax, strides));

  if (use_tos)
    set_mean_on_01face(K, parent, S, tmp, K2::is_face_2);
  else
    set_mean_on_01face(K, parent, S, tmp, K1::is_face_2);

  io::imsave(tmp, "tmp.tiff");
  {
    image2d<attr_2f> x;
    image2d<attr_1f> y;
    std::tie(x,y) = morpho::tos_compute_attribute(K, parent, S, tmp, attr_2f(), attr_1f());

    io::imsave(transform(y, [=](attr_1f v) -> float { return (float)v.size; }),     "clength.tiff");
    io::imsave(transform(y, [=](attr_1f v) -> float { return (float)v.gradient; }), "cgradient.tiff");

    float mu = std::atof(argv[4]);
    simplify(K, parent, S, x, y, mu);

    image2d<rgb8> out;
    if (use_tos)
      out = setmean_on_nodes(tmp, K, parent, S, K2::is_face_2);
    else
      out = setmean_on_nodes(tmp, K, parent, S, K1::is_face_2);

    io::imsave(tmp, "simplify_0.tiff");
    io::imsave(out, "simplify_1.tiff");
  }

  exit(0);

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
