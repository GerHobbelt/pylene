#pragma once

#include <mln/core/box.hpp>
#include <mln/core/extension/private/by_periodize.hpp>
#include <mln/core/image/view/extend_by_projection.hpp>
#include <mln/core/private/traits/extension.hpp>

#include <mln/core/rangev3/view/transform.hpp>
#include <concepts/concepts.hpp>

namespace mln
{
  template <class I>
  class periodize_extended_view;

  namespace view
  {
    template <class I>
    periodize_extended_view<I> periodize_extended(I image);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {
    template <class D>
    struct periodize_projector
    {
      auto operator()(const typename D::point_type& p) const { return m_domain.periodize(p); }
      D m_domain;
    };
  };


  template <class I>
  class periodize_extended_view
    : public extend_by_projection_view_base<I, details::periodize_projector<image_domain_t<I>>>,
      public experimental::Image<periodize_extended_view<I>>
  {
    using base = extend_by_projection_view_base<I, details::periodize_projector<image_domain_t<I>>>;
  public:
    periodize_extended_view(I image)
      : base(image, {image.domain()})
    {
    }

    using extension_type = extension::by_periodize;
    extension_type extension() const { return {}; }
  };


  namespace view
  {

    template <class I>
    periodize_extended_view<I> periodize_extended(I image)
    {
      static_assert(::concepts::convertible_to<image_domain_t<I>, mln::experimental::Box>,
                    "Periodize requires image to be defined over a regular box domain");
      return periodize_extended_view<I>{std::move(image)};
    }
  }; // namespace view

} // namespace mln
