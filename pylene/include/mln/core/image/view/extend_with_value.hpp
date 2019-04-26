#pragma once

#include <mln/core/extension/extension_traits.hpp>

#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <mln/core/image/morphers/extended_by_value_image.hpp> // mln::by_value_extension<V>

#include <range/v3/utility/common_type.hpp> // common_reference

namespace mln
{
  template <class I>
  class extended_by_value_view;

  namespace view
  {
    template <class I>
    extended_by_value_view<I> extend_with_value(I image, image_value_t<I> value);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class extended_by_value_view : public image_adaptor<I>, public experimental::Image<extended_by_value_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::value_extension_tag;
    using extension_type     = by_value_extension<image_value_t<I>>;
    using reference          = ::ranges::common_reference_t<image_reference_t<I>, image_value_t<I>&>;
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = extended_by_value_view::reference;

      reference val() const { return m_ima->domain().has(this->base().point()) ? this->base().val() : m_ima->m_value; }

      new_pixel_type(image_pixel_t<I> px, extended_by_value_view* ima)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_ima{ima}
      {
      }

    private:
      extended_by_value_view* m_ima;
    };


    extended_by_value_view(I ima, image_value_t<I> value)
      : base_t{std::move(ima)}
      , m_value{std::move(value)}
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
      return this->domain().has(p) ? this->base().at(p) : m_value;
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel(p), this};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [ptr = this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), ptr};
      });
    }

    extension_type extension() { return {&m_value}; }

  private:
    image_value_t<I> m_value;
  };


  namespace view
  {
    template <class I>
    extended_by_value_view<I> extend_with_value(I image, image_value_t<I> value)
    {
      return {std::move(image), std::move(value)};
    }
  }; // namespace view

} // namespace mln
