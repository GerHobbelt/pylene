#pragma once


#include <mln/morpho/experimental/maxtree.hpp>
#include <mln/morpho/experimental/private/immersion.hpp>
#include <mln/morpho/experimental/private/propagation.hpp>
#include <mln/core/image/view/cast.hpp>

namespace mln::morpho::experimental
{

  enum {
    ToS_NodeMapOriginalSize,
    ToS_NodeMapTwiceSize
  };

  /// \brief Compute the tree of shapes.
  /// \param[in] input Input image
  /// \param[in] start_point Root point
  /// \return A pair (tree, node_map) encoding the tree of shapes and the mapping (pixel -> node_index)
  template <class I>
  [[gnu::noinline]] auto tos(I input, image_point_t<I> pstart, int processing_flags = ToS_NodeMapTwiceSize);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  auto tos(I input, image_point_t<I> pstart, int /* processing_flags */)
  {
    static_assert(mln::is_a<I, mln::experimental::Image>());

    using Domain = image_domain_t<I>;
    using P = image_point_t<I>;

    static_assert(std::is_same_v<Domain, mln::experimental::box2d> || std::is_same_v<Domain, mln::experimental::box3d>,
                  "Only 2D or 3D regular domain supported");

    using connectivity_t = std::conditional_t<P::ndim == 2, mln::experimental::c4_t, mln::experimental::c6_t>;
    connectivity_t nbh;

    int max_depth;
    auto [inf, sup] = details::immersion(input);

    image_ch_value_t<I, int> ord = imchvalue<int>(inf).adjust(nbh);
    details::propagation(inf, sup, ord, pstart, max_depth);

    if (max_depth >= (1 << 16))
      throw std::runtime_error("The ToS is too deep (too many number of levels)");

    auto casted = mln::view::cast<uint16_t>(ord);
    return maxtree(casted, nbh);
  }

} // namespace mln::morpho::experimental
