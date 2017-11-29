#ifndef TOS_TESTS_HELPER_HPP
# define TOS_TESTS_HELPER_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/morpho/datastruct/component_tree.hpp>


/// \brief Compute the parent image representation of the tree
/// The root a component is set to the mininum point of the component to ensure uniqueness

mln::image2d<mln::point2d>
tree_as_parent_image(const mln::morpho::component_tree<unsigned, mln::image2d<unsigned>>& tree,
                     mln::image2d<mln::point2d>& roots);




#endif //!TOS_TESTS_HELPER_HPP
