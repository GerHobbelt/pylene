#ifndef MLN_CORE_MORPHO_MERGE_TREE_HPP
# define MLN_CORE_MORPHO_MERGE_TREE_HPP

namespace mln
{

  namespace morpho
  {


    // After tree merge, (parent | domain) needs canonization
    // (parent | domain)c is still canonized
    template <typename V, typename StrictWeakOrdering>
    void merge_tree(const image2d<V>& ima,
		    image2d<point2d>& parent,
		    box2d domain,
		    StrictWeakOrdering cmp)
    {
      mln_precondition(domain.empty());

      point2d p;
      point2d q;
      p[0] = domain.pmin[0]-1;
      q[0] = domain.pmin[0];

      unsigned ncols = ima.ncols();
      for (unsigned i = 0; i < ncols; ++i)
	{
	  p[1] = q[1] = i;
	  point2d rp = ima(p) == ima(parent(p)) ? parent(p) : p;
	  point2d rq = ima(q) == ima(parent(q)) ? parent(q) : q;
	  // check that parent is canonized
	  mln_assertion(rp == parent(rp) or ima(parent(rp)) != ima(rp));
	  mln_assertion(rq == parent(rq) or ima(parent(rq)) != ima(rq));

	  while (rp != rq)
	    {
	      if (cmp(ima(rq), ima(rp))) // attach zpar(rp) to rq
		{
		  point2d par = parent(rp);
		  while (cmp(ima(rq), ima(par))) {
		    rp = par;
		    par = parent(rp);
		  }
		  parent(rp) = rq;
		  rp = par;
		}
	      else if (cmp(ima(rp), ima(rq))) // attach zpar(rq) to rp
		{
		  point2d par = parent(rq);
		  while (cmp(ima(rp), ima(par))) {
		    rq = par;
		    par = parent(rq);
		  }
		  parent(rq) = rp;
		  rq = par;
		}
	      else
		{
		  point2d par = parent(rq);
		  parent(rq) = rp;
		  rp = par;
		}
	    }

	}
    }

  }

}


#endif // !MLN_CORE_MORPHO_MERGE_TREE_HPP
