#ifndef MLN_CORE_MORPHO_MAXTREE_PQUEUE_HPP
# define MLN_CORE_MORPHO_MAXTREE_PQUEUE_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/extension/fill.hpp>
# include <mln/core/wrt_offset.hpp>
# include <vector>
# include <stack>
# include <queue>


#include <mln/io/imprint.hpp>

namespace mln
{

  namespace morpho
  {

    namespace internal
    {

      template <typename V, typename Neighborhood, typename StrictWeakOrdering>
      void
      maxtree_flood_pqueue_algorithm(const image2d<V>& ima,
				     image2d<std::size_t>& parent,
				     const Neighborhood& , StrictWeakOrdering cmp)
      {
	image2d<bool> deja_vu;
	typedef std::vector<std::size_t> vec_t;
	vec_t v_stack; v_stack.reserve(ima.domain().size());
	vec_t v_pqueue; v_pqueue.reserve(ima.domain().size());
	auto fcmp = [&ima, cmp] (std::size_t x, std::size_t y) { return cmp(ima[x], ima[y]); };
	std::stack<std::size_t, vec_t> stack(std::move(v_stack));
	std::priority_queue<std::size_t, vec_t, decltype(fcmp)>
	  pqueue(fcmp, std::move(v_pqueue));
	auto nbh_delta_indexes = wrt_delta_index(ima, Neighborhood::dpoints);

	// INIT
	{
	  resize(deja_vu, ima, (ima.index_strides()[0] - ima.ncols()) / 2, false);
	  extension::fill(deja_vu, true);

	  // Get min element and reserve queue
	  std::size_t pmin = ima.index_of_point(ima.domain().pmin);
	  V vmin = ima[pmin];
	  {
	    mln_pixter(px, ima);
	    mln_forall(px)
	    {
	      if (cmp(px->val(), vmin))
		{
		  vmin = px->val();
		  pmin = px->index();
		}
	    }

	    pqueue.push(pmin);
	    stack.push(pmin);
	    deja_vu[pmin] = true;
	  }
	}

	// Flood
	while (!pqueue.empty())
	  {
	    std::size_t p = pqueue.top();
	    std::size_t repr = stack.top();

	    if (cmp(ima[p], ima[repr]))
	      {
		// the component represented by repr is over
		// we attach repr to its parent
		stack.pop();
		assert(!stack.empty());
		std::size_t par = stack.top();
		while (cmp(ima[p], ima[par]))
		  {
		    stack.pop();
		    parent[repr] = par;
		    repr = par;
		    par = stack.top();
		  }
		if (cmp(ima[par], ima[p])) { // ima[par] < ima[p]
		  stack.push(p);
		  par = p;
		}
		parent[repr] = par;
		repr = par;
	      }


	    assert(ima[repr] == ima[p]);
	    bool done = true;
	    mln_foreach(auto k, nbh_delta_indexes)
	      {
		auto q = p + k;
		if (!deja_vu[q]) {
		  pqueue.push(q);
		  deja_vu[q] = true;
		  if (cmp(ima[p], ima[q])) {
		    stack.push(q);
		    done = false;
		    break;
		  }
		}
	      }

	    if (done) {
	      parent[p] = repr;
	      pqueue.pop();
	    }
	  }

      }

    } // end of namespace mln::morpho::internal


    template <typename V, typename Neighborhood, typename StrictWeakOrdering = std::less<V> >
    image2d<std::size_t>
    maxtree_pqueue(const image2d<V>& ima, const Neighborhood& nbh, StrictWeakOrdering cmp = StrictWeakOrdering())
    {
      image2d<std::size_t> parent;
      resize(parent, ima);

      internal::maxtree_flood_pqueue_algorithm(ima, parent, nbh, cmp);
      return parent;
    }

  }  // end of namespace mln::morpho

} // end of namespace mln

#endif // ! MLN_CORE_MORPHO_MAXTREE_PQUEUE_HPP
