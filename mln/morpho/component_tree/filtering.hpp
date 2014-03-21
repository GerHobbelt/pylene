#ifndef MLN_MORPHO_COMPONENT_TREE_FILTERING_HPP
# define MLN_MORPHO_COMPONENT_TREE_FILTERING_HPP

# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/trace.hpp>

namespace mln
{

  namespace morpho
  {

    /// \brief Min filtering strategy
    /// A node is remove if it does not pass the predicate and
    /// all its descendant are removed as well.
    template <class P, class Amap, class PredicateMap>
    void
    filter_min_inplace(component_tree<P, Amap>& tree,
		       const PredicateMap& predicate_map);

    /// \brief Direct filtering strategy
    /// A node is remove if it does not pass the predicate, the others
    /// remain.
    template <class P, class Amap, class PredicateMap>
    void
    filter_direct_inplace(component_tree<P, Amap>& tree,
			  const PredicateMap& predicate_map);


    template <class P, class Amap, class PredicateMap, class ValueMap, class OutputImage>
    void
    filter_direct_and_reconstruct(const component_tree<P, Amap>& tree,
				  const PredicateMap& predicate_map,
				  const ValueMap& value_map,
				  OutputImage&& out);



    template <class P, class Amap, class PredicateMap>
    void
    filter_max_inplace(component_tree<P, Amap>& tree,
		       const PredicateMap& predicate_map);


    /********************/
    /** Implementation **/
    /********************/

    template <class P, class Amap, class PredicateMap>
    void
    filter_min_inplace(component_tree<P, Amap>& tree,
		       const PredicateMap& pred)
    {
      mln_entering("mln::morpho::filter_min_inplace");

      typedef component_tree<P, Amap> tree_t;
      typedef typename tree_t::vertex_id_t vertex_id_t;

      auto data = tree._get_data();

      property_map<tree_t, vertex_id_t> newrepr(tree);

      // forward
      for (auto x = tree.get_root(); x.id() != tree.npos(); )
	if (not pred[x.id()])
	  {
	    vertex_id_t q = x.get_parent_id();
	    data->m_nodes[x.get_prev_node_id()].m_next = x.get_next_sibling_id();
	    data->m_nodes[x.get_next_sibling_id()].m_prev = x.get_prev_node_id();

	    // unactive subtree.
	    auto end = x.next_sibling();
	    for (; x != end; x = x.next_node())
	      newrepr[x.id()] = q;
	  }
	else
	  {
	    newrepr[x.id()] = x.id();
	    x = x.next_node();
	  }

      // backward to update next_sibling relation
      mln_reverse_foreach(auto x, tree.nodes())
	if (not pred[x.get_next_sibling_id()])
	  data->m_nodes[x.id()].m_next_sibling = x.next_sibling().get_next_sibling_id();

      // reaffect point to nodes
      mln_foreach (auto& v, data->m_pmap.values())
	v = newrepr[v];

      mln_exiting();
    }

    template <class P, class Amap, class PredicateMap>
    void
    filter_direct_inplace(component_tree<P, Amap>& tree,
			  const PredicateMap& pred)
    {
      mln_entering("mln::morpho::filter_direct_inplace");

      typedef component_tree<P, Amap> tree_t;
      typedef typename tree_t::vertex_id_t vertex_id_t;

      auto data = tree._get_data();

      property_map<tree_t, vertex_id_t> newrepr(tree);

      // forward
      for (auto x = tree.get_root(); x.id() != tree.npos(); x = x.next_node())
	if (not pred[x.id()])
	  {
	    vertex_id_t q = x.get_parent_id();
	    data->m_nodes[x.get_prev_node_id()].m_next = x.get_next_node_id();
	    data->m_nodes[x.get_next_node_id()].m_prev = x.get_prev_node_id();

	    // unactive this node.
	    newrepr[x.id()] = newrepr[q];
	  }
	else
	  {
	    newrepr[x.id()] = x.id();
	  }

      // backward to update next_sibling relation
      mln_reverse_foreach(auto x, tree.nodes())
	if (not pred[x.get_next_sibling_id()])
	  data->m_nodes[x.id()].m_next_sibling = x.next_sibling().get_next_node_id();

      // reaffect point to nodes
      mln_foreach (auto& v, data->m_pmap.values())
	v = newrepr[v];

      mln_exiting();
    }


    template <class P, class Amap, class PredicateMap, class ValueMap, class OutputImage>
    void
    filter_direct_and_reconstruct(const component_tree<P, Amap>& tree,
				  const PredicateMap& predicate_map,
				  const ValueMap& value_map,
				  OutputImage&& out)
    {
      mln_entering("mln::morpho::filter_direct_and_reconstruct");

      typedef component_tree<P, Amap> tree_t;
      typedef typename tree_t::vertex_id_t vertex_id_t;

      ValueMap vmap = value_map;

      mln_foreach(auto x, tree.nodes())
	if (not predicate_map[x])
	  vmap[x] = vmap[x.parent()];

      mln_foreach(auto px, out.pixels())
	{
	  auto x = tree.get_node_at(px.index());
	  px.val() = vmap[x];
	}

      mln_exiting();
    }


  }

}

#endif // ! MLN_MORPHO_COMPONENT_TREE_FILTERING_HPP
