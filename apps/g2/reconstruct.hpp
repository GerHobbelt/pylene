#ifndef RECONSTRUCT_HPP
# define RECONSTRUCT_HPP

# include <mln/morpho/component_tree/filtering.hpp>
# include <boost/foreach.hpp>
# include "types.hpp"

namespace mln
{

  template <class Graph, class AMap>
  void
  reconstruct_marginal(const tree_t& t1, const tree_t& t2, const tree_t& t3,
		       const AMap& val1, const AMap& val2, const AMap& val3,
		       const Graph& graph,
		       image2d<rgb8>& out);


  /***************************/
  /**   Implementation      **/
  /***************************/


  template <class Graph, class AMap>
  void
  reconstruct_marginal(const tree_t& t1, const tree_t& t2, const tree_t& t3,
		       const AMap& val1, const AMap& val2, const AMap& val3,
		       const Graph& graph,
		       image2d<rgb8>& out)
  {
    auto glinks = boost::get(&graph_content::tlinks, graph);
    const tree_t* t_array[3] = {&t1, &t2, &t3};
    //const AMap* val_array[3] = {&val1, &val2, &val3};

    // 1. Get the active nodes in the trees from graph nodes.
    property_map<tree_t, bool> active[3] = {
      property_map<tree_t, bool> (t1, false),
      property_map<tree_t, bool> (t2, false),
      property_map<tree_t, bool> (t3, false)
    };

    BOOST_FOREACH(typename Graph::vertex_descriptor v, boost::vertices(graph))
      {
	for (int i = 0; i < 3; ++i)
	  if (glinks[v][i] != t_array[i]->nend() )
	    active[i][glinks[v][i]] = true;
      }


    filter_direct_and_reconstruct(t1, active[0], val1,
				  imtransform(out, [](rgb8& x) -> uint8& { return x[0]; }));

    filter_direct_and_reconstruct(t2, active[1], val2,
				  imtransform(out, [](rgb8& x) -> uint8& { return x[1]; }));

    filter_direct_and_reconstruct(t3, active[2], val3,
				  imtransform(out, [](rgb8& x) -> uint8& { return x[2]; }));
  }

}

#endif // ! RECONSTRUCT_HPP
