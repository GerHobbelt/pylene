#ifndef APPS_G2_COMPUTE_G2_HPP
# define APPS_G2_COMPUTE_G2_HPP

# include "types.hpp"

namespace mln
{
  /// \brief Compute the graph g2
  /// \returns the tuple (G, T₁, T₂, T₃) where
  ///          + G is the graph of shapes
  ///          + T₁ is the property map linking the first tree to a graph node
  ///          + T₂ is the property map linking the second tree to a graph node
  ///          + T₃ is the property map linking the third tree to a graph node
  std::tuple<Graph,
             property_map<tree_t, Graph::vertex_descriptor>,
             property_map<tree_t, Graph::vertex_descriptor>,
             property_map<tree_t, Graph::vertex_descriptor> >
  compute_g2(const tree_t& t1,
             const tree_t& t2,
             const tree_t& t3);


  /// \brief Generic version of the procedure to compute g2
  /// \param trees A vector of size NTREE containing the marginal trees
  std::tuple<Graph, std::array<property_map<tree_t, Graph::vertex_descriptor>, NTREE> >
  compute_g2(const tree_t* trees);
}

#endif // ! APPS_G2_COMPUTE_G2_HPP
