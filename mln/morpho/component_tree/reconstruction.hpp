#ifndef MLN_MORPHO_COMPONENT_TREE_RECONSTRUCTION_HPP
# define MLN_MORPHO_COMPONENT_TREE_RECONSTRUCTION_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/trace.hpp>

# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/morpho/datastruct/attribute_map.hpp>


namespace mln
{

  namespace morpho
  {

    template <class P, class AMap, class V, class I>
    void reconstruction(const component_tree<P, AMap>& tree,
			const property_map< component_tree<P, AMap>, V >& pmap,
			Image<I>& out);


    /**************************/
    /** Implementation	     **/
    /**************************/

    namespace impl
    {
      template <class P, class AMap, class V, class I>
      void reconstruction_index(const component_tree<P, AMap>& tree,
				const property_map< component_tree<P, AMap>, V >& pmap,
				I& out)
      {
	mln_pixter(px, out);
	mln_forall(px)
	  px->val() = pmap[tree.get_node_id(px->index())];
      }

    }

    template <class P, class AMap, class V, class I>
    void reconstruction(const component_tree<P, AMap>& tree,
			const property_map< component_tree<P, AMap>, V >& pmap,
			Image<I>& out)
    {
      mln_entering("mln::morpho::reconstruction");
      impl::reconstruction_index(tree, pmap, exact(out));
      mln_exiting();
    }




  }

}

#endif // ! MLN_MORPHO_COMPONENT_TREE_RECONSTRUCTION_HPP
