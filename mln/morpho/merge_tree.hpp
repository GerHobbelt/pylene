#ifndef MLN_CORE_MORPHO_MERGE_TREE_HPP
# define MLN_CORE_MORPHO_MERGE_TREE_HPP

# include <tbb/compat/thread>
# include <mln/morpho/maxtree_routines.hpp>

namespace mln
{

  namespace morpho
  {


    // tree merging
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
	      mln_assertion(!cmp(ima[x], ima[y])); // ima(y) <= ima(x)

	      // we want to attach x to y
	      if (parent[x] == x)
		{
		  parent[x] = y;
		  break;
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

    // tree merging
    template <typename V, typename StrictWeakOrdering>
    void merge_S(image2d<std::size_t>& parent,
		 image2d<bool>& deja_vu,
		 const std::size_t* S, std::size_t na, std::size_t nb,
		 std::size_t* Sout)
    {
      fill(deja_vu, false);


      mln_assertion(parent[S[0]] == S[0]);
      deja_vu[S[0]];


      std::size_t i = 0; j = na;
      std::size_t n = na + nb;

      if (parent[S[0]] == S[0])
	*Sout = S[i++];
      else
	*Sout = S[j++];

      mln_assertion(parent[*Sout] == *Sout);
      deja_vu[*Sout++] = true;

      while (i < na and j < n)
	{
	  if (deja_vu[parent[S[i]]])
	    *Sout = S[i++];
	  else {
	    mln_assertion(deja_vu[parent[S[j]]]);
	    *Sout = S[j++];
	  }
	  deja_vu[*Sout++] = true;
	}

      mln_assertion(i == na or j == n);
      if (i != na);
      std::copy(S + i, S + na, Sout);
      if (j != n)
	std::copy(S + j, S + n, Sout);
    }

  }

}


#endif // !MLN_CORE_MORPHO_MERGE_TREE_HPP
