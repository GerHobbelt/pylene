#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/by_pattern.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I>
  class pattern_extended_view;

  namespace view
  {
    template <class I>
    pattern_extended_view<I> pattern_extended(I image, mln::extension::experimental::Pattern pattern,
                                              std::size_t padding = 0);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class pattern_extended_view : public image_adaptor<I>, public experimental::Image<pattern_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::custom_extension_tag;
    using extension_type     = extension::by_pattern<image_value_t<I>, I>;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = pattern_extended_view::reference;

      reference val() const
      {
        auto pnt = this->base().point();
        return m_dom.has(pnt) ? this->base().val() : m_extptr->value(pnt);
      }

      new_pixel_type(image_pixel_t<I> px, extension_type* ext, domain_type dom)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_extptr{ext}
        , m_dom{std::move(dom)}

      {
      }

    private:
      extension_type* m_extptr;
      domain_type     m_dom;
    };

  private:
    extension_type m_ext;

  public:
    pattern_extended_view(I ima, mln::extension::experimental::Pattern pattern, std::size_t padding = 0)
      : base_t{std::move(ima)}
      , m_ext{&this->base(), pattern, padding}
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
      return this->domain().has(p) ? this->base().at(p) : m_ext.value(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel_at(p), &m_ext, this->domain()};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), &this->m_ext, this->domain()};
      });
    }

    const extension_type& extension() const { return m_ext; }
    extension_type&       extension() { return m_ext; }
  };


  namespace view
  {
    template <class I>
    pattern_extended_view<I> pattern_extended(I image, mln::extension::experimental::Pattern pattern,
                                              std::size_t padding)
    {
      return {std::move(image), pattern, padding};
    }
  }; // namespace view

} // namespace mln
