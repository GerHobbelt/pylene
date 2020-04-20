#pragma once

#include <mln/core/box.hpp>
#include <mln/core/private/traits/extension.hpp>
#include <mln/core/extension/private/by_clamp.hpp>
#include <mln/core/image/view/extend_by_projection.hpp>

#include <mln/core/range/view/transform.hpp>
#include <concepts/concepts.hpp>

namespace mln
{
  template <class I>
  class clamp_extended_view;

  namespace view
  {
    template <class I>
    clamp_extended_view<I> clamp_extended(I image);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class D>
    struct clamp_projector
    {
      auto operator()(const typename D::point_type& p) const { return m_domain.clamp(p); }
      D m_domain;
    };
  };


  template <class I>
  class clamp_extended_view : public extend_by_projection_view_base<I, details::clamp_projector<image_domain_t<I>>>,
                              public mln::details::Image<clamp_extended_view<I>>
  {
    using base = extend_by_projection_view_base<I, details::clamp_projector<image_domain_t<I>>>;
  public:
    clamp_extended_view(I image)
      : base(image, {image.domain()})
    {
    }

    using extension_type = extension::by_clamp;
    extension_type extension() const { return {}; }
  };


  namespace view
  {

    template <class I>
    clamp_extended_view<I> clamp_extended(I image)
    {
      static_assert(::concepts::convertible_to<image_domain_t<I>, mln::Box>,
                    "Clamp requires image to be defined over a regular box domain");
      return clamp_extended_view<I>{std::move(image)};
    }
  }; // namespace view

} // namespace mln
