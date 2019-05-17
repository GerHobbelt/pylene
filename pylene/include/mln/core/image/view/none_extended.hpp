#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/none.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I>
  class none_extended_view;

  namespace view
  {
    template <class I>
    none_extended_view<I> none_extended(I image, image_value_t<I> value);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class none_extended_view : public image_adaptor<I>, public experimental::Image<none_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::experimental::value_tag;
    using extension_type     = extension::none;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = none_extended_view::reference;

      reference val() const
      {
        if (!m_dom.has(this->base().point()))
          throw std::runtime_error("Accessing point out of bound!");

        return this->base().val();
      }

      new_pixel_type(image_pixel_t<I> px, domain_type dom)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_dom{std::move(dom)}
      {
      }

    private:
      domain_type m_dom;
    };

  private:
    extension_type m_ext;

  public:
    explicit none_extended_view(I ima)
      : base_t{std::move(ima)}
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
      if (!this->domain().has(p))
        throw std::runtime_error("Accessing point out of bound!");

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
      return {this->base().new_pixel(p), this->domain()};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), this->domain()};
      });
    }

    const extension_type& extension() { return m_ext; }
  };


  namespace view
  {
    template <class I>
    none_extended_view<I> none_extended(I image)
    {
      return {std::move(image)};
    }
  }; // namespace view

} // namespace mln
