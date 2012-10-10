#ifndef MAXTREE_UFIND_HPP
# define MAXTREE_UFIND_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/algorithm/sort_indexes.hpp>
# include <mln/core/wrt_offset.hpp>
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

    std::size_t
    zfind_root(image2d<std::size_t>& parent, std::size_t p)
    {
      if (parent[p] == p)
        return p;
      else
        return parent[p] = zfind_root(parent, parent[p]);
    }

  template <typename V, typename Neighborhood>
  image2d<std::size_t>
  maxtree(const image2d<V>& ima, const Neighborhood& nbh)
  {
    image2d<std::size_t> parent, zpar;
    image2d<bool> deja_vu;
    resize(parent, ima);
    resize(zpar, ima);
    resize(deja_vu, ima, ima.border(), false);

    extension::fill(deja_vu, true);

    std::vector<std::size_t> v = sort_indexes(ima);
    auto offsets = wrt_delta_index(ima, nbh.dpoints);

    for (std::size_t p: v)
      {
        std::cout << p << std::endl;
	// make set
	{
	  parent[p] = p;
	  zpar[p] = p;
	  deja_vu[p] = true;
	}

	for (unsigned k = 0; k < offsets.size(); ++k)
	  {
	    std::size_t q = p + offsets[k];
	    if (deja_vu[q])
	      {
                std::size_t r = zfind_root(zpar, q);
		if (r != p) // make union
		  {
		    parent[r] = p;
		    zpar[r] = p;
		  }
	      }
	  }
      }

    // canonization
    for (int i = v.size()-1; i >= 0; --i)
      {
	std::size_t p = v[i];
	std::size_t q = parent[p];
	if (ima[parent[q]] == ima[q])
	  parent[p] = parent[q];
      }

    return parent;
  }

  }

}

#endif // !MLN_MORPHO_MAXTREE_UFIND_HPP