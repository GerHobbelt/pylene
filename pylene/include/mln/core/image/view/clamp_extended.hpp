#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/extension/private/by_clamp.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference


namespace mln
{
  template <class I>
  class clamp_extended_view;

  namespace view
  {
    template <class I>
    clamp_extended_view<I> clamp_extended(I image);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace detail
  {
    template <typename Coord>
    Coord compute_clamped_coords_impl(const Coord& pnt, std::size_t shp_)
    {
      if (shp_ == 0)
        throw std::runtime_error("Division by zero!");

      if (pnt < 0)
        return 0;

      auto shp = static_cast<Coord>(shp_);

      return std::min(pnt, static_cast<Coord>(shp - 1));
    }

    template <typename Point, std::size_t... Idx>
    Point compute_clamped_coords(const Point& pnt, const Point& shp, std::index_sequence<Idx...>)
    {
      return {compute_clamped_coords_impl(pnt[Idx], shp[Idx])...};
    }
  } // namespace detail


  template <class I>
  class clamp_extended_view : public image_adaptor<I>, public experimental::Image<clamp_extended_view<I>>
  {
    using base_t = image_adaptor<I>;

  public:
    using extension_category = extension::custom_extension_tag;
    using extension_type     = extension::by_clamp;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;

    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, experimental::Pixel<new_pixel_type>
    {
      using reference = clamp_extended_view::reference;

      reference val() const { return (*m_ima)(this->base().point()); }

      new_pixel_type(image_pixel_t<I> px, clamp_extended_view<I>* ima)
        : new_pixel_type::pixel_adaptor{std::move(px)}
        , m_ima{ima}
      {
      }

    private:
      clamp_extended_view<I>* m_ima;
    };

  public:
    explicit clamp_extended_view(I ima)
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
      return this->domain().has(p) ? this->base().at(p) : compute_clamped_value(p);
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

    extension_type extension() const { return {}; }

  private:
    reference compute_clamped_value(const point_type& pnt) const
    {
      PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<domain_type>,
                               "Domain must be shaped to allow pattern-based extension!");

      return this->base()(
          detail::compute_clamped_coords(pnt, this->domain().shape(), std::make_index_sequence<domain_type::ndim>{}));
    }
  };


  namespace view
  {
    template <class I>
    clamp_extended_view<I> clamp_extended(I image)
    {
      return clamp_extended_view<I>{std::move(image)};
    }
  }; // namespace view

} // namespace mln