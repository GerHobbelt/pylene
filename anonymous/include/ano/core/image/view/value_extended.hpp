#pragma once

#include <ano/core/extension/private/by_value.hpp>
#include <ano/core/image/view/adaptor.hpp>
#include <ano/core/private/traits/extension.hpp>
#include <ano/core/range/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace ano
{
  template <class I>
  class value_extended_view;

  namespace view
  {
    template <class I>
    value_extended_view<I> value_extended(I image, image_value_t<I> value);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class value_extended_view : public image_adaptor<I>, public ano::details::Image<value_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using point_type         = image_point_t<I>;
    using extension_category = extension::value_extension_tag;
    using extension_type     = extension::by_value<image_value_t<I>>;
    using value_type         = image_value_t<I>;

    // FIXME: use common_reference here between image_reference_t<I> AND value_type&
    using reference = image_reference_t<I>; // Not write only (we may want an out of bound value while keeping the
                                            // the writability of the image)

    using category_type = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using typename image_adaptor<I>::domain_type;

    struct pixel_type : pixel_adaptor<image_pixel_t<I>>, ano::details::Pixel<pixel_type>
    {
      using reference = value_extended_view::reference;

      reference val() const
      {
        auto pnt = this->base().point();
        return m_dom.has(pnt) ? this->base().val() : m_ima->m_value;
      }

      pixel_type(image_pixel_t<I> px, value_extended_view<I>* ima, domain_type dom)
        : pixel_type::pixel_adaptor{std::move(px)}
        , m_ima{ima}
        , m_dom{std::move(dom)}
      {
      }

    private:
      value_extended_view<I>* m_ima;
      domain_type             m_dom;
    };

  private:
    mutable image_value_t<I> m_value;

  public:
    value_extended_view(I ima, image_value_t<I> value)
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
    std::enable_if_t<image_accessible_v<J>, pixel_type> pixel(point_type p)
    {
      return this->pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, pixel_type> pixel_at(point_type p)
    {
      return {this->base().pixel_at(p), this, this->domain()};
    }
    /// \}

    auto pixels()
    {
      return ranges::view::transform(this->base().pixels(), [this](image_pixel_t<I> px) -> pixel_type {
        return {std::move(px), this, this->domain()};
      });
    }

    extension_type extension() const { return extension_type{&this->m_value}; }
  };


  namespace view
  {
    template <class I>
    value_extended_view<I> value_extended(I image, image_value_t<I> value)
    {
      return {std::move(image), std::move(value)};
    }
  }; // namespace view

} // namespace ano
