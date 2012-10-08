#ifndef MAXTREE_UFIND_HPP
# define MAXTREE_UFIND_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
//# include <tbb/parallel_reduce.h>

namespace mln
{

  namespace morpho
  {

  namespace impl
  {

    // template <typename V, typename Neighborhood>
    // struct MaxTreeAlgorithmUF
    // {
    //   MaxTreeAlgorithmUF(const image2d<V>& ima, const Neighborhood& nbh)
    // 	: m_ima (ima), m_nbh (nbh)
    //   {
    //   }


    //   std::tuple< image2d<unsigned>, image2d<unsigned>, box2d >
    //   operator() (const box2d& domain)
    //   {
    //   }


    // private:
    //   const image2d<V>& m_ima;
    //   Neighborhood m_nbh;
    //   image2d<unsigned> parent;
    //   image2d<unsigned> zpar;
    // };

  }


  template <typename V, typename Neighborhood>
  image2d<std::ptrdiff_t>
  maxtree(const image2d<V>& ima, const Neighborhood& nbh)
  {
    image2d<ptrdiff_t> parent, zpar;
    image2d<bool> deja_vu;
    resize(parent, ima);
    resize(zpar, ima);
    resize(deja_vu, ima, false);

    extension::fill(deja_vu, true);

    std::vector<unsigned> v = sort_offset(f);

    auto offsets = wrt_offset(ima, nbh.dpoints);
    for (std::ptrdiff_t p: v)
      {
	// make set
	{
	  parent.element(p) = p;
	  zpar.element(p) = p;
	  deja_vu.element(p) = true;
	}

	for (int k = 0; k < offsets.size(); ++k)
	  {
	    std::ptrdiff_t q = p + offsets[k];
	    if (deja_vu.element(q))
	      {
		r = zfind_root(zpar, q);
		if (r != p) // make union
		  {
		    parent.element(r) = p;
		    zpar.element(r) = p;
		  }
	      }
	  }
      }

    // canonization
    for (int i = v.size()-1; i >= 0; --i)
      {
	std::ptrdiff_t p = v[i];
	std::ptrdiff_t q = parent.element(p);
	if (ima.element(parent.element(q)) == ima.element(q))
	  parent.element(p) = parent.element(q);
      }

    return parent;
  }

  }

}

#endif // !MLN_MORPHO_MAXTREE_UFIND_HPP
