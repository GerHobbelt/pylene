#pragma once

#include <ano/core/box.hpp>
#include <ano/core/extension/private/by_mirror.hpp>
#include <ano/core/image/view/extend_by_projection.hpp>
#include <ano/core/private/traits/extension.hpp>

#include <ano/core/range/view/transform.hpp>
#include <concepts/concepts.hpp>

namespace ano
{
  template <class I>
  class mirror_extended_view;

  namespace view
  {
    template <class I>
    mirror_extended_view<I> mirror_extended(I image);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class D>
    struct mirror_projector
    {
      auto operator()(const typename D::point_type& p) const { return m_domain.mirror(p); }
      D    m_domain;
    };
  }; // namespace details


  template <class I>
  class mirror_extended_view : public extend_by_projection_view_base<I, details::mirror_projector<image_domain_t<I>>>,
                               public ano::details::Image<mirror_extended_view<I>>
  {
    using base = extend_by_projection_view_base<I, details::mirror_projector<image_domain_t<I>>>;

  public:
    mirror_extended_view(I image)
      : base(image, {image.domain()})
    {
    }

    using extension_type = extension::by_mirror;
    extension_type extension() const { return {}; } // FIXME
  };


  namespace view
  {

    template <class I>
    mirror_extended_view<I> mirror_extended(I image)
    {
      static_assert(::concepts::convertible_to<image_domain_t<I>, ano::Box>,
                    "Clamp requires image to be defined over a regular box domain");
      return mirror_extended_view<I>{std::move(image)};
    }
  }; // namespace view

} // namespace ano
