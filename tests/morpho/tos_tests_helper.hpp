#ifndef TOS_TESTS_HELPER_HPP
# define TOS_TESTS_HELPER_HPP


# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/core/image/image2d.hpp>
# include <mln/core/image/image3d.hpp>

/// \brief Compute the parent image representation of the tree
/// The root a component is set to the mininum point of the component to ensure uniqueness

template <class ParentImage>
ParentImage
tree_as_parent_image(const mln::morpho::component_tree<typename ParentImage::index_type, mln_ch_value(ParentImage, unsigned)>& tree,
                     ParentImage& roots);



using tree_2d_t = mln::morpho::component_tree<mln::NDImageInterface::index_type, mln::ndimage_interleaved<2, unsigned> >;
using tree_3d_t = mln::morpho::component_tree<mln::NDImageInterface::index_type, mln::ndimage_interleaved<3, unsigned> >;
using par_2d_t = mln::ndimage_interleaved<2, typename mln::ImageNdInterface<2>::point_type>;
using par_3d_t = mln::ndimage_interleaved<3, typename mln::ImageNdInterface<3>::point_type>;


extern template
par_2d_t tree_as_parent_image(const tree_2d_t& tree, par_2d_t& roots);

extern template
par_3d_t tree_as_parent_image(const tree_3d_t& tree, par_3d_t& roots);



#endif //!TOS_TESTS_HELPER_HPP
