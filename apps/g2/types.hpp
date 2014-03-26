#ifndef TYPES_HPP
# define TYPES_HPP

# include <mln/morpho/component_tree/component_tree.hpp>
# include <mln/core/image/image2d.hpp>
# include <mln/core/vec.hpp>
# include <boost/graph/adjacency_list.hpp>
# include <array>

enum { NTREE = 3 }; // We merge 3 trees

typedef mln::morpho::component_tree<unsigned, mln::image2d<unsigned> > tree_t;
struct tree_node_t { typedef boost::vertex_property_tag kind; };

struct graph_content
{
  std::array<tree_t::node_type, NTREE> tlinks;
  tree_t::node_type ulink; // A unique link among one of the three trees
  mln::vec3u depth; // The depth of the SES in the three trees.
};

typedef boost::adjacency_list<boost::setS,
			      boost::vecS,
			      boost::directedS,
			      graph_content> Graph;

typedef mln::property_map<tree_t, Graph::vertex_descriptor> tlink_t;

#endif // ! TYPES_HPP


