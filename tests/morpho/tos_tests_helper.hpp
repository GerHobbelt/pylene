#ifndef TOS_TESTS_HELPER_HPP
# define TOS_TESTS_HELPER_HPP


# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/core/image/image.hpp>

/// \brief Compute the parent image representation of the tree
/// The root a component is set to the mininum point of the component to ensure uniqueness

template <class ParentImage>
ParentImage
tree_as_parent_image(const mln::morpho::component_tree<unsigned, mln_ch_value(ParentImage, unsigned)>& tree,
                     ParentImage& roots);





#endif //!TOS_TESTS_HELPER_HPP
