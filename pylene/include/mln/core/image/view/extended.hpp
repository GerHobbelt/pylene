#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/image_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/pattern_extended.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference

#include <variant>


namespace mln
{
  template <class I, class... Args>
  class extended_view;

  namespace view
  {
    template <class I, class... Args>
    extended_view<I> extended(I image, Args&&... args);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I, class... Args>
  class extended_view : public image_adaptor<I>, public experimental::Image<extended_view<I>>
  {
    using base_t          = image_adaptor<I>;
    using adapted_image_t = std::variant<I, // can be the base image (se fit, no adaptation required)
                                         image_extended_view<I, Args...>, none_extended_view<I>,
                                         pattern_extended_view<I, Args{}...>, extended_view<I>>;

  public:
    using extension_category = extension::experimental::value_tag;
    // FIXME
    // using extension_type     = extension::by_value<image_value_t<I>>;
    using reference     = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type    = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;
    using typename image_adaptor<I>::new_pixel_type;

  private:
    adapted_image_t m_adapted_image;

  public:
    extended_view(I ima, adapted_image_t adapted_image)
      : base_t{std::move(ima)}
      , m_adapted_image{adapted_image}
    {
    }

    /// Accessible-image related methods
    /// \{
    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> operator()(point_type p)
    {
      return this->at(p);
    }


    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> at(point_type p)
    {
      return this->base().at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return this->base().new_pixel(p);
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), &this->m_ext, this->domain()};
      });
    }

    // const extension_type& extension() { return m_ext; }
  };


  namespace view
  {
    template <class I, class... Args>
    extended_view<I> extended(I image, Args&&... args)
    {
      return {std::move(image), std::forward<Args>(args)...};
    }
  }; // namespace view

} // namespace mln
