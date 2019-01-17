#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <type_traits>


namespace mln
{

  template <class I, class D>
  class clip_view : public image_adaptor<I>, public experimental::Image<clip_view<I, D>>
  {
    D m_domain;

  public:
    /// Type definitions
    /// \{
    using typename clip_view::image_adaptor::new_pixel_type;
    using typename clip_view::image_adaptor::point_type;
    using typename clip_view::image_adaptor::reference;
    using typename clip_view::image_adaptor::value_type;
    using domain_type   = D;
    using concrete_type = clip_view<image_concrete_t<I>, D>;

    template <class V>
    using ch_value_type = clip_view<image_ch_value_t<I, V>, D>;
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = std::true_type;
    using indexable          = typename I::indexable;
    using extension_category = mln::extension::none_extension_tag; // FIXME: should be improved
    /// \}


    // Concepts check
    static_assert(std::is_convertible_v<typename D::value_type, point_type>,
                  "Domain value type must be convertible to image point type.");

    static_assert(I::accessible::value, "Image must be pw-accessible.");


  public:
    clip_view(I ima, D domain)
      : clip_view::image_adaptor{std::move(ima)}
      , m_domain{std::move(domain)}
    {
      // FIXME: possibly assert that domain is included in ima.domain()
    }

    template <class I2, class D2>
    clip_view(const clip_view<I2, D2>& other, mln::init)
      : clip_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()))}
      , m_domain{other.m_domain}
    {
    }

    template <class I2, class D2>
    clip_view(const clip_view<I2, D2>& other, const value_type& v)
      : clip_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()).init(v))}
      , m_domain{other.m_domain}
    {
    }

    internal::initializer<concrete_type, clip_view> concretize() const { return {*this}; }

    template <class U>
    internal::initializer<ch_value_type<U>, clip_view> ch_value() const
    {
      return {*this};
    }

    const D& domain() const { return this->m_domain; }

    auto new_values()
    {
      auto g = [this](point_type p) -> reference { return this->base().at(p); };
      return mln::ranges::view::transform(this->m_domain, g);
    }

    auto new_pixels()
    {
      auto g = [this](point_type p) -> new_pixel_type { return this->base().pixel_at(p); };
      return mln::ranges::view::transform(this->m_domain, g);
    }

    reference operator()(point_type p)
    {
      mln_precondition(m_domain.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().at(p);
    }

    reference at(point_type p) { return this->base().at(p); }


    new_pixel_type new_pixel(point_type p)
    {
      mln_precondition(this->m_domain.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->clip_view::image_adaptor::new_pixel_at(p);
    }

    new_pixel_type new_pixel_at(point_type p) { return this->clip_view::image_adaptor::new_pixel_at(p); }
  };


  namespace view
  {
    template <class I, class D>
    clip_view<I, D> clip(I ima, D domain)
    {
      static_assert(mln::is_a<I, experimental::Image>());

      // FIXME: make ima a view first ?
      return {std::move(ima), std::move(domain)};
    }

  } // namespace view

} // namespace mln
