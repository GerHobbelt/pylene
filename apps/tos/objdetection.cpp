# include <mln/core/image/image2d.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/wrt_offset.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/morpho/tos/irange.hpp>
# include <mln/morpho/tos/immerse.hpp>
# include <mln/morpho/tos/pset.hpp>
# include <mln/morpho/tos/tos.hpp>
# include <mln/core/neighb2d.hpp>
# include <mln/io/imread.hpp>
# include <mln/core/trace.hpp>

#include "topology.hpp"
#include "Kinterpolate.hpp"


namespace mln
{

    struct energy_t {
      bool dejavu = false;
      int depth = -1;
      int m_e_length = 0;
      float m_e_sumcurv = 0;
      int m_v_n_int = 0;
      int m_v_sum_int = 0;
      int m_v_sum_int_sqr = 0;
      int m_v_n_ext = 0;
      int m_v_sum_ext = 0;
      int m_v_sum_ext_sqr = 0;

      float energy() const
      {
	const float alpha = 60;
	const float beta = 2;
	auto sqr = [](float x) { return x*x; };

	float Vin = m_v_sum_int_sqr - sqr(m_v_sum_int) / m_v_n_int;
	float Vout = m_v_n_ext == 0 ? 0 : m_v_sum_ext_sqr - sqr(m_v_sum_ext) / m_v_n_ext;
	float Eext = (Vin + Vout) / ((m_v_sum_int_sqr + m_v_sum_ext_sqr) -
				     sqr(m_v_sum_int + m_v_sum_ext) / (m_v_n_ext + m_v_n_int));

	float Eint = m_e_sumcurv / m_e_length;
	float Econ = 1 / m_e_length;
	return alpha * Eint + Eext + beta * Econ;
      }

      friend
      std::ostream&
      operator <<(std::ostream& os, const energy_t& acc)
      {
	return os << "Contour length: " <<  acc.m_e_length << std::endl
		  << "Curvature: " << acc.m_e_sumcurv << std::endl
		  << "Internal Vertex: " << acc.m_v_n_int << std::endl
		  << "External Vertex: " << acc.m_v_n_ext << std::endl
		  << "Energy: " << acc.energy() << std::endl;
      }

    };




  /// \pre x and y must be canonical elements
  unsigned
  common_ancestor(unsigned x, unsigned y, const image2d<energy_t>& e, const image2d<unsigned>& parent)
  {
    while (x != y) {
      if (e[x].depth > e[y].depth)
	x = parent[x];
      else
	y = parent[y];
    }
    return x;
  }

  template <typename Iterator, typename V>
  unsigned
  common_ancestor(Iterator x, const image2d<energy_t>& e, const image2d<unsigned>& parent, const image2d<V>& K)
  {
    std::vector<unsigned> v;
    int mindepth = value_traits<int>::max();
    mln_forall(x) {
      if (!K.domain().has(x->point()))
	continue;

      unsigned i = x->index();
      i = (K[i] == K[parent[i]]) ? parent[i] : i;
      v.push_back(i);
      if (e[i].depth < mindepth)
	mindepth = e[i].depth;
    }

    bool modif = true;
    while (modif)
      {
	modif = false;
	for(unsigned& i: v)
	  if (e[i].depth > mindepth)
	    {
	      i = parent[i];
	      modif = true;
	      if (e[i].depth < mindepth)
		mindepth = e[i].depth;
	    }

	if (!modif) {
	  bool alleq = std::all_of(v.begin(), v.end(), [&](unsigned x) { return x == v[0]; });
	  if (!alleq)
	    for(unsigned& i: v)
	      {
		i = parent[i];
		modif = true;
		if (e[i].depth < mindepth)
		  mindepth = e[i].depth;
	      }
	}
      }

    return v[0];
  }

  template <typename V>
  image2d<float>
  curvature_on_edge(const image2d<V>& ima)
  {
    typedef decltype(V() + V()) Vec;

    trace::entering("mln::curvature_on_edge");

    //auto norm = [] (const Vec& x) -> int { return std::abs(x[0] + x[1] + x[2]); };
    auto norm = [] (const Vec& x) -> int { return std::abs(x); };
    auto sqr = [] (float x) -> float { return x*x; };

    auto domain = ima.domain();
    domain.pmin *= 2;
    domain.pmax = domain.pmax * 2 - 1;
    image2d<float> curv(domain, ima.border(), 0);

    mln_foreach(const point2d& p, ima.domain())
      {
	// Right edge
	{
	  int ux = norm((ima.at(p + point2d{0,1}) - ima.at(p)));
	  int uy = norm( (ima.at(p + point2d{1,0}) - ima.at(p + point2d{-1,0}) +
			  ima.at(p + point2d{1,1}) - ima.at(p + point2d{-1,1})) / 4 );

	  //std::cout << p << " " << ux << " " << (- ima.at(p + point2d{-1,0})) << ":" << uy << std::endl;

	  int uxx = norm( ima.at(p + point2d{0,-1}) - ima.at(p)
			  -ima.at(p + point2d{0,1}) + ima.at(p + point2d{0,2}) / 2);

	  int uyy = norm( ima.at(p + point2d{-1,0}) + ima.at(p + point2d{-1,1})
			  -2 * (ima.at(p) + ima.at(p + point2d{0,1}))
			  + ima.at(p + point2d{1,0}) + ima.at(p + point2d{1,1}) / 2);

	  int uxy = norm( ( ima.at(p + point2d{1,0}) - ima.at(p + point2d{-1,0}) +
			    ima.at(p + point2d{1,1}) - ima.at(p + point2d{-1,1})) / 2 );

	  float den = (sqr(ux) + sqr(uy));
	  point2d p_ = p * 2 + point2d{0,1};
	  if (den != 0)
	    curv.at(p_) = std::abs((uxx * sqr(uy) - 2 * uxy * ux *uy + uyy * sqr(ux)) / (den * std::sqrt(den)));
	  else
	    curv.at(p_) = 0;
	}

	// Bottom edge
	{
	  int uy = norm((ima.at(p + point2d{1,0}) - ima.at(p)));

	  int ux = norm( (ima.at(p + point2d{0,1}) - ima.at(p + point2d{0,-1}) +
			  ima.at(p + point2d{1,1}) - ima.at(p + point2d{1,-1})) / 4 );

	  int uxx = norm( ima.at(p + point2d{-1,0}) - ima.at(p)
			  -ima.at(p + point2d{1,0}) + ima.at(p + point2d{2,0}) / 2);

	  int uyy = norm( ima.at(p + point2d{0,-1}) + ima.at(p + point2d{1,-1})
			  -2 * (ima.at(p) + ima.at(p + point2d{1,0}))
			  + ima.at(p + point2d{0,1}) + ima.at(p + point2d{1,1}) / 2);

	  int uxy = norm( ( ima.at(p + point2d{0,-1}) - ima.at(p + point2d{1,0})
			   -ima.at(p + point2d{1,-1}) + ima.at(p + point2d{1,1})) / 2 );

	  float den = (sqr(ux) + sqr(uy));
	  point2d p_ = p * 2 + point2d{1,0};
	  if (den != 0)
	    curv.at(p_) = std::abs((uxx * sqr(uy) - 2 * uxy * ux *uy + uyy * sqr(ux)) / (den * std::sqrt(den)));
	  else
	    curv.at(p_) = 0;
	}
      }

    trace::exiting();
    return curv;
  }

  template <typename V>
  image2d<float>
  close(const image2d<float>& energy, const image2d<V>& K, const image2d<unsigned>& parent, const std::vector<unsigned>& S)
  {
    image2d<float> imdilate = clone(energy);

    // Dilate
    for (int i = S.size()-1; i >= 0; --i)
      {
	unsigned x = S[i];
	unsigned q = parent[x];
	if (K[x] != K[q])
	  {
	    imdilate[x] = std::max(imdilate[x], energy[q]);
	    imdilate[q] = std::max(imdilate[q], energy[x]);
	  }
      }

    // Erode
    image2d<float> imerode = clone(imdilate);
    for (int i = S.size()-1; i >= 0; --i)
      {
	unsigned x = S[i];
	unsigned q = parent[x];
	if (K[x] != K[q])
	  {
	    imerode[x] = std::min(imerode[x], imdilate[q]);
	    imerode[q] = std::min(imerode[q], imdilate[x]);
	  }
      }

    return imerode;
  }


  template <typename V>
  image2d<float>
  compute_energy(const image2d<V>& ima, const image2d<V>& K, image2d<unsigned>& parent, const std::vector<unsigned>& S,
		 int eps = 5)
  {
    extension::fill(ima, ima(ima.domain().pmin));

    image2d<float> curv = curvature_on_edge(ima);

    trace::entering("mln::compute_energy");

    image2d<energy_t> acc;
    resize(acc, K);

    //io::imprint(ima);
    //io::imprint(curv);

    // Compute depth attribute
    acc[S[0]].depth = 0;
    for (unsigned i = 1; i < S.size(); ++i)
      {
	unsigned x = S[i];
	if (K[x] != K[parent[x]]) // canonical element
	  acc[x].depth = acc[parent[x]].depth + 1;
      }


    // Compute attribute about edges
    {
      auto c4_idx = wrt_delta_index(curv, c4.dpoints);
      acc[S[0]].m_e_length = 4;
      for (int i = S.size()-1; i > 0; --i)
	{
	  unsigned x = S[i];
	  if (K1::is_face_2(K.point_at_index(x))) {
	    acc[x].m_e_length += 4;
	    mln_foreach(int i, c4_idx) {
	      mln_assertion(K1::is_face_1(curv.point_at_index(x + i)));
	      acc[x].m_e_sumcurv += curv[x + i];
	    }
	  }
	  else if (K1::is_face_1(K.point_at_index(x))) {
	    acc[x].m_e_length += -2;
	    acc[x].m_e_sumcurv -= 2 * curv[x];
	  }
	  acc[parent[x]].m_e_length += acc[x].m_e_length;
	  acc[parent[x]].m_e_sumcurv += acc[x].m_e_sumcurv;
	}
    }



    typedef iterator_range< stditerator<std::vector<point2d>::const_iterator> > Vec;
    typedef nbh< Vec > Nbh;
    Nbh ball;
    std::vector<point2d> dpoints;
    for (int i = -eps*2; i <= eps*2; i += 2)
      for (int j = -eps*2; j <= eps*2; j += 2)
	dpoints.emplace_back(i,j);

    ball.dpoints = make_iterator_range(stditerator< std::vector<point2d>::const_iterator >(dpoints.begin(),dpoints.end()));

    // FIX TO HANDLE THE ROOT CORRECTLY
    parent[S[0]] = (unsigned) -1;

    // Compute attribute about internal area of components
    {
      mln_pixter(px, K);
      mln_iter(nx, ball(px));
      mln_forall(px)
      {
	if (K1::is_face_2(px->point()))
	  {
	    unsigned x = px->index();
	    x = (x != S[0] and K[x] == K[parent[x]]) ? parent[x] : x;

	    unsigned y = 0;
	    mln_forall(nx) {
	      if (!K.domain().has(nx->point()))
		{
		  y = (unsigned) -1; // This pixel is on the border
		  break;
		}
	    }

	    if (y == 0)
	      y = common_ancestor(nx, acc, parent, K);

	    while (x != y)
	      {
		// p in Xint
		acc[x].m_v_n_int += 1;
		acc[x].m_v_sum_int += px->val();
		acc[x].m_v_sum_int_sqr += px->val() * px->val();
		x = parent[x];
	      }
	  }
      }
    }

    // RESTORE ROOT
    parent[S[0]] = S[0];

    // Compute attribute about external area of components
    {
      mln_pixter(px, K);
      mln_iter(nx, ball(px));

      std::vector< std::pair<unsigned, unsigned> > branches;
      mln_forall(px)
      {
	if (K1::is_face_2(px->point()))
	  {
	    unsigned r = px->index();
	    r = (K[r] == K[parent[r]]) ? parent[r] : r;
	    branches.clear();

	    mln_forall(nx)
	    {
	      if (!K.domain().has(nx->point()))
		continue;

	      unsigned x = nx->index();
	      x = (K[x] == K[parent[x]]) ? parent[x] : x;

	      unsigned y = common_ancestor(x, r, acc, parent);
	      branches.emplace_back(x,y);

	      while (x != y)
		{
		  if (acc[x].dejavu) {
		    x = parent[x];
		    continue;
		  }
		  // p in Xint
		  acc[x].m_v_n_ext += 1;
		  acc[x].m_v_sum_ext += px->val();
		  acc[x].m_v_sum_ext_sqr += px->val() * px->val();
		  acc[x].dejavu = true;
		  x = parent[x];
		}
	    }

	    unsigned x,y;
	    for (auto v: branches) {
	      std::tie(x,y) = v;
	      while (x != y) {
		acc[x].dejavu = false;
		x = parent[x];
	      }
	    }

	  }
      }
    }

    
    // Display
    {
      image2d<bool> bdebug;
      resize(bdebug, K).init(false);

      for (int i = S.size()-1; i >= 0; --i)
	{
	  unsigned p = S[i];
	  bdebug[p] = true;
	  if (p == parent[p] or K[p] != K[parent[p]]) {
	    std::cout << "=================" << std::endl
		      << "Process: " << p << std::endl << acc[p] << std::endl;
	    Kdisplay(bdebug);
	  }
	}
    }


    // make enery image
    image2d<float> energy;
    resize(energy, acc);
    {
      for (unsigned x : S)
	{
	  if (K[x] != K[parent[x]] or x == parent[x])
	    energy[x] = acc[x].energy();
	}
    }

    trace::exiting();

    return energy;
  }


}




int main(int argc, char** argv)
{
  if (argc != 2)
    std::cerr << "Usage: " << argv[0] << " ima.pgm" << std::endl;

  using namespace mln;
  image2d<uint8> ima;
  io::imread(argv[1], ima);


  auto f = interpolate_median(ima, UInt<9> ());

  image2d< UInt<9> > K;
  std::vector<unsigned> S;
  image2d<unsigned> parent;
  std::tie(K, parent, S) = morpho::ToS(f, c4);

  image2d<float> energy = compute_energy(f, K, parent, S);

  io::imprint(energy);
  energy = close(energy, K, parent, S);
  io::imprint(energy);
}
