#ifndef MLN_MORPHO_MAXTREE_MAXTREE_HPP
# define MLN_MORPHO_MAXTREE_MAXTREE_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/trace.hpp>
# include <mln/core/neighborhood/neighborhood.hpp>
# include <mln/morpho/maxtree/maxtree_queue.hpp>

namespace mln
{

  namespace morpho
  {
    ///
    /// \brief Compute a maxtree as a component tree where nodes contain indexes instead of points.
    ///
    ///
    template <typename I, typename N, typename StrictWeakOrdering = std::less<mln_value(I)> >
    component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
    maxtree_indexes(const Image<I>& ima, const Neighborhood<N>& nbh,
		    StrictWeakOrdering cmp = StrictWeakOrdering());


    /*****************************/
    /**  Implementation		**/
    /*****************************/

    template <typename I, typename N, typename StrictWeakOrdering>
    component_tree<typename I::size_type, mln_ch_value(I, unsigned)>
    maxtree_indexes(const Image<I>& ima, const Neighborhood<N>& nbh, StrictWeakOrdering cmp)
    {
      mln_entering("mln::morpho::maxtree_indexes");
      auto res = impl::maxtree_queue_indexes(exact(ima), exact(nbh), cmp);
      mln_exiting();
      return res;
    }


  }

}

#endif // ! MLN_MORPHO_MAXTREE_MAXTREE_HPP