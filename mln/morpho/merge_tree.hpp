#ifndef MLN_CORE_MORPHO_MERGE_TREE_HPP
# define MLN_CORE_MORPHO_MERGE_TREE_HPP

# include <tbb/compat/thread>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {

      template <typename V>
      point2d
      zfind_repr(const image2d<V>& ima, image2d<point2d>& parent, const point2d& p)
      {
	point2d q = parent(p);
	if (q != p and ima(q) == ima(p))
	  return parent(p) = zfind_repr(ima, parent, q);
	else
	  return p;
      }

      template <typename V>
      unsigned
      zfind_repr(const image2d<V>& ima, image2d<std::size_t>& parent,
		 unsigned p)
      {
	unsigned q = parent[p];
	if (q != p and ima[q] == ima[p])
	  return parent[p] = zfind_repr(ima, parent, q);
	else
	  return p;
      }

      template <typename V>
      point2d
      zfind_parent(const image2d<V>& ima, image2d<point2d>& parent, const point2d& p)
      {
	point2d q = parent(p);
	point2d r = parent(q);
	if (q != r and ima(q) == ima(r))
	  return parent(p) = zfind_repr(ima, parent, r);
	else
	  return q;
      }


    }


    // After tree merge, (parent | domain) needs canonization
    // (parent | domain)c is still canonized
    template <typename V, typename StrictWeakOrdering>
    void merge_tree(const image2d<V>& ima,
		    image2d<point2d>& parent,
		    box2d domain,
		    StrictWeakOrdering cmp)
    {
      mln_precondition(!domain.empty());

      point2d p;
      point2d q;
      p[0] = domain.pmax[0]-1;
      q[0] = domain.pmax[0];

      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < ncols; ++i)
	{
	  p[1] = q[1] = i;
	  //std::cout << "Merge: " << p << " @ " << q << std::endl;
	  point2d x = internal::zfind_repr(ima, parent, p);
	  point2d y = internal::zfind_repr(ima, parent, q);
	  if (cmp(ima(x), ima(y)))
	    std::swap(x, y);

	  while (x != y)
	    {
	      // check that x and y are representative
	      mln_assertion(x == parent(x) or ima(parent(x)) != ima(x));
	      mln_assertion(y == parent(y) or ima(parent(y)) != ima(y));
	      mln_assertion(!cmp(ima(x), ima(y)));

	      // we want to attach x to y
	      if (parent(x) == x)
		{
		  parent(x) = y;
		  x = y;
		}
	      else
		{
		  point2d z = internal::zfind_parent(ima, parent, x);
		  if (!cmp(ima(z), ima(y)))
		    x = z;
		  else
		    {
		      //std::cout << "Connect " << x << "/" << (int)ima(x) << " @ " << y << "/" << (int) ima(y) << std::endl;
		      parent(x) = y;
		      x = y;
		      y = z;
		    }
		}

	      // if (cmp(ima(rq), ima(rp))) // attach zpar(rp) to rq
	      // 	{
	      // 	  point2d par = internal::zfind_parent(ima, parent, rp);
	      // 	  while (par != rp and cmp(ima(rq), ima(par))) {
	      // 	    rp = par;
	      // 	    par = internal::zfind_parent(ima, parent, rp);
	      // 	  }
	      // 	  parent(rp) = rq;
	      // 	  if (par == rp)
	      // 	    break;
	      // 	  rp = par;
	      // 	}
	      // else if (cmp(ima(rp), ima(rq))) // attach zpar(rq) to rp
	      // 	{
	      // 	  point2d par = internal::zfind_parent(ima, parent, rq);
	      // 	  while (par != rq and cmp(ima(rp), ima(par))) {
	      // 	    rq = par;
	      // 	    par = internal::zfind_parent(ima, parent, rq);
	      // 	  }
	      // 	  parent(rq) = rp;
	      // 	  if (par == rq)
	      // 	    break;
	      // 	  rq = par;
	      // 	}
	      // else
	      // 	{
	      // 	  point2d par = internal::zfind_parent(ima, parent, rq);
	      // 	  parent(rq) = rp;
	      // 	  if (par == rq)
	      // 	    break;
	      // 	  rq = par;
	      // 	}
	    }

	}
    }

  }

}


#endif // !MLN_CORE_MORPHO_MERGE_TREE_HPP
