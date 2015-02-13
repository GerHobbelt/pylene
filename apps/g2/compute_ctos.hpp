#ifndef APPS_G2_COMPUTE_CTOS_HPP
# define APPS_G2_COMPUTE_CTOS_HPP

# include <mln/core/image/image2d.hpp>
# include <mln/core/colors.hpp>
# include <mln/core/grays.hpp>
# include <mln/morpho/component_tree/component_tree.hpp>

namespace mln
{


  /// \brief A convenient wrapper around:
  /// * The Graph of shapes computation + depth attribute valuation
  /// * The Saturated Maxtree algorithm
  ///
  /// \param input The input image
  /// \param[out] depth if not NULL, store the depth image inside.
  morpho::component_tree<unsigned, image2d<unsigned> >
  compute_ctos(const image2d<rgb8>& input,
               image2d<uint16>* depth = nullptr);


}

#endif // ! APPS_G2_COMPUTE_CTOS_HPP
