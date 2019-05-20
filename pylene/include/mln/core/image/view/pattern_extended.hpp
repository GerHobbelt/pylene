#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/by_pattern.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I, mln::extension::experimental::Pattern P>
  class pattern_extended_view;

  namespace view
  {
    template <class I, mln::extension::experimental::Pattern P>
    pattern_extended_view<I, P> pattern_extended(I image, std::size_t padding = 0);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I, mln::extension::experimental::Pattern P>
  class pattern_extended_view : public image_adaptor<I>, public experimental::Image<pattern_extended_view<I, P>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::experimental::pattern_tag;
    using extension_type     = extension::by_pattern<image_value_t<I>, P>;
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
        return m_dom.has(pnt) ? this->base().val() : m_extptr->value(pnt, *m_imaptr);
      }

      new_pixel_type(image_pixel_t<I> px, extension_type* ext, const I* ima, domain_type dom)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_extptr{ext}
        , m_imaptr(ima)
        , m_dom{std::move(dom)}

      {
      }

    private:
      extension_type* m_extptr;
      I*              m_imaptr;
      domain_type     m_dom;
    };

  private:
    extension_type m_ext;

  public:
    pattern_extended_view(I ima, mln::extension::experimental::Pattern pattern, std::size_t padding = 0)
      : base_t{std::move(ima)}
      , m_ext{pattern, padding}
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
      return this->domain().has(p) ? this->base().at(p) : m_ext.value(p, this->base());
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel(p), &this->m_ext, &this->base(), this->domain()};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), &m_ext, &this->base(), this->domain()};
      });
    }

    const extension_type& extension() { return m_ext; }
  };


  namespace view
  {
    template <class I, mln::extension::experimental::Pattern P>
    pattern_extended_view<I, P> pattern_extended(I image, std::size_t padding)
    {
      return {std::move(image), padding};
    }
  }; // namespace view

} // namespace mln
