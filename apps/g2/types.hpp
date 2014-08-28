#ifndef TYPES_HPP
# define TYPES_HPP

# include <mln/morpho/component_tree/component_tree.hpp>
# include <mln/core/image/image2d.hpp>
# include <mln/core/colors.hpp>
# include <mln/colors/rgba.hpp>
# include <mln/core/vec.hpp>
# include <boost/graph/adjacency_list.hpp>
# include <array>

#ifndef MLN_INPUT_VALUE_TYPE
# define MLN_INPUT_VALUE_TYPE mln::rgb8
#endif

typedef MLN_INPUT_VALUE_TYPE value_t;

enum { NTREE = value_t::ndim }; // We merge 3 trees

typedef mln::morpho::component_tree<unsigned, mln::image2d<unsigned> > tree_t;
struct tree_node_t { typedef boost::vertex_property_tag kind; };

struct graph_content
{
  std::array<tree_t::node_type, NTREE> tlinks;
  tree_t::node_type ulink; // A unique link among one of the three trees
  mln::vec<unsigned, NTREE> depth; // The depth of the SES in the trees.
  mln::vec<unsigned, NTREE> senc;  // The id of the smallest enclosing shape (⊆) in each tree
                                   // tlinks[i] ≠ None ⇒ senc[i] = tlinks[i].id()
};

typedef boost::adjacency_list<boost::setS,
			      boost::vecS,
			      boost::directedS,
			      graph_content> Graph;

typedef mln::property_map<tree_t, Graph::vertex_descriptor> tlink_t;

#endif // ! TYPES_HPP


