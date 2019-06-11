#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/by_mirror.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I>
  class mirror_extended_view;

  namespace view
  {
    template <class I>
    mirror_extended_view<I> mirror_extended(I image, std::size_t padding = 0);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I>
  class mirror_extended_view : public image_adaptor<I>, public experimental::Image<mirror_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::custom_extension_tag;
    using extension_type     = extension::by_mirror<image_value_t<I>, I>;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = mirror_extended_view::reference;

      reference val() const
      {
        auto pnt = this->base().point();
        return m_dom.has(pnt) ? this->base().val() : m_ima->compute_mirrored_value(pnt);
      }

      new_pixel_type(image_pixel_t<I> px, mirror_extended_view<I>* ima, domain_type dom)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_ima{ima}
        , m_dom{std::move(dom)}

      {
      }

    private:
      mirror_extended_view<I>* m_ima;
      domain_type              m_dom;
    };

  private:
    using const_extension_type = extension::by_mirror<image_value_t<I>, I, false>;
    std::size_t m_padding;

  public:
    explicit mirror_extended_view(I ima, std::size_t padding = 0)
      : base_t{std::move(ima)}
      , m_padding{padding}
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
      return this->domain().has(p) ? this->base().at(p) : compute_mirrored_value(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return {this->base().new_pixel_at(p), this, this->domain()};
    }
    /// \}

    auto new_pixels()
    {
      return ranges::view::transform(this->base().new_pixels(), [this](image_pixel_t<I> px) -> new_pixel_type {
        return {std::move(px), this, this->domain()};
      });
    }

    const_extension_type extension() const { return const_extension_type{&this->m_padding}; }
    extension_type       extension() { return extension_type{&this->m_padding}; }

  private:
    reference compute_mirrored_value(const point_type& pnt) const
    {
      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_type>,
                               "Domain must be shaped to allow pattern-based extension!");

      return this->base()(
          compute_coords(pnt, this->domain().shape(), std::make_index_sequence<domain_type::dimension>{}));
    }

    template <std::size_t... Idx>
    point_type compute_coords(const point_type& pnt, const point_type& shp, std::index_sequence<Idx...>) const
    {
      if ((((shp[Idx] - m_padding) <= 0) || ...))
        throw std::runtime_error(
            "Cannot have a padding whose size is supperior or equal to one of the dimension's shape.");

      return {compute_coords_impl(pnt[Idx], shp[Idx])...};
    }

    template <typename Coord>
    Coord compute_coords_impl(Coord pnt, std::size_t shp_) const
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      auto shp = static_cast<Coord>(shp_);

      if (pnt < 0)
        pnt -= m_padding;
      else if (pnt >= shp)
        pnt += m_padding;

      while (pnt < (2 * shp))
        pnt += 2 * shp;

      pnt %= 2 * shp;

      if (pnt < shp)
        return pnt;
      else // pnt >= shp
        return 2 * shp - 1 - pnt;
    }
  };


  namespace view
  {
    template <class I>
    mirror_extended_view<I> mirror_extended(I image, std::size_t padding)
    {
      return mirror_extended_view<I>{std::move(image), padding};
    }
  }; // namespace view

} // namespace mln
