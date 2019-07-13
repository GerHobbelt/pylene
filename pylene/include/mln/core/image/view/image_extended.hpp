#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/by_image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I>
  class image_extended_view;

  namespace view
  {
    template <class I, class U>
    image_extended_view<I> image_extended(I image, U base_ima, image_point_t<U> offset = image_point_t<U>{});
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class image_extended_view : public image_adaptor<I>, public experimental::Image<image_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::image_extension_tag;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;
    using typename image_adaptor<I>::value_type;
    using extension_type = extension::by_image<value_type, point_type>;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = image_extended_view::reference;

      reference val() const { return (*m_ima)(this->base().point()); }

      new_pixel_type(image_pixel_t<I> px, image_extended_view<I>* ima)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_ima{ima}
      {
      }

    private:
      image_extended_view<I>* m_ima;
    };

  private:
    mutable std::function<bool(const point_type&)>      m_has_value_func;
    mutable std::function<reference(const point_type&)> m_get_value_func;

  public:
    template <typename U>
    image_extended_view(I ima, U base_ima, point_type offset)
      : base_t{std::move(ima)}
      , m_has_value_func{[base_ima, offset](const point_type& pnt) { return base_ima.domain().has(pnt + offset); }}
      , m_get_value_func{[base_ima, offset](const point_type& pnt) { return base_ima(pnt + offset); }}
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
      return this->domain().has(p) ? this->base().at(p) : get_value_from_extension(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel_at(p), this};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), this};
      });
    }

    extension_type extension() const { return {&m_has_value_func, &m_get_value_func}; }

  private:
    reference get_value_from_extension(const point_type& pnt) const
    {
      if (!m_has_value_func(pnt))
        throw std::runtime_error("Accessing point out of bound !");

      return m_get_value_func(pnt);
    }
  };


  namespace view
  {
    template <class I, class U>
    image_extended_view<I> image_extended(I image, U base_ima, image_point_t<U> offset)
    {
      return {std::move(image), std::move(base_ima), std::move(offset)};
    }
  }; // namespace view

} // namespace mln
