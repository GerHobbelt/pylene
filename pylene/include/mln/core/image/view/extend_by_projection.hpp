#pragma once

#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/range/view/transform.hpp>
#include <mln/core/private/traits/extension.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I, class Proj>
  class extend_by_projection_view_base;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class I, class Proj>
  class extend_by_projection_view_base : public image_adaptor<I>
  {
    using base_t = image_adaptor<I>;
    Proj m_proj;

  public:
    using extension_category = extension::custom_extension_tag;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, mln::details::Pixel<new_pixel_type>
    {
      using reference = extend_by_projection_view_base<I, Proj>::reference;

      reference val() const
      {
        auto q = std::invoke(m_view->m_proj, this->base().point());
        return (*m_view)(q);
      }

      new_pixel_type(image_pixel_t<I> px, extend_by_projection_view_base<I, Proj>* ima)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_view{ima}
      {
      }

    private:
      extend_by_projection_view_base<I, Proj>* m_view;
    };

  public:
    extend_by_projection_view_base<I, Proj>(I ima, Proj proj)
      : base_t{std::move(ima)}
      , m_proj{std::move(proj)}
    {
    }


    /// Accessible-image related methods
    /// \{
    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> operator()(point_type p)
    {
      assert(this->base().domain().has(p));
      return this->at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> at(point_type p)
    {
      auto q = std::invoke(m_proj, p);
      return this->base().at(q);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      assert(this->base().domain().has(p));
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel_at(p), this};
    }
    /// \}

    auto pixels()
    {
      return ranges::view::transform(this->base().pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), this};
      });
    }
  };

} // namespace mln
