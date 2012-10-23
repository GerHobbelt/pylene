#ifndef MLN_CORE_MORPHO_MERGE_TREE_HPP
# define MLN_CORE_MORPHO_MERGE_TREE_HPP

# include <tbb/compat/thread>
# include <mln/morpho/maxtree_routines.hpp>

namespace mln
{

  namespace morpho
  {


    // After tree merge, (parent | domain) needs canonization
    // (parent | domain)c is still canonized
    template <typename V, typename StrictWeakOrdering>
    void merge_tree(const image2d<V>& ima,
		    image2d<std::size_t>& parent,
		    box2d domain,
		    StrictWeakOrdering cmp)
    {
      mln_precondition(!domain.empty());

      point2d p_ = domain.pmin;
      point2d q_ = domain.pmin;
      p_[0] = domain.pmax[0]-1;
      q_[0] = domain.pmax[0];
      std::size_t p = ima.index_of_point(p_);
      std::size_t q = ima.index_of_point(q_);

      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < ncols; ++i, ++p, ++q)
	{
	  std::size_t x = internal::zfind_repr(ima, parent, p);
	  std::size_t y = internal::zfind_repr(ima, parent, q);
	  if (cmp(ima[x], ima[y]))
	    std::swap(x, y);

	  while (x != y)
	    {
	      //std::cout << "-- Merge: " << x << " @ " << y << std::endl;
	      // check that x and y are representative
	      mln_assertion(x == parent[x] or ima[parent[x]] != ima[x]);
	      mln_assertion(y == parent[y] or ima[parent[y]] != ima[y]);
	      mln_assertion(!cmp(ima[x], ima[y]));

	      // we want to attach x to y
	      if (parent[x] == x)
		{
		  parent[x] = y;
		  x = y;
		}
	      else
		{
		  std::size_t z = internal::zfind_parent(ima, parent, x);
		  if (!cmp(ima[z], ima[y]))
		    x = z;
		  else
		    {
		      parent[x] = y;
		      x = y;
		      y = z;
		    }
		}
	    }

	}

    }

  }

}


#endif // !MLN_CORE_MORPHO_MERGE_TREE_HPP
