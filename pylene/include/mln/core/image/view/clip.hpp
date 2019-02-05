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
    using domain_type = D;

    static_assert(std::is_convertible_v<::ranges::range_value_t<D>, point_type>,
                  "Domain value type must be convertible to image point type.");
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = std::true_type;
    using indexable          = image_indexable_t<I>;
    using view               = std::true_type;
    using extension_category = mln::extension::none_extension_tag; // FIXME: should be improved
    using category_type      = forward_image_tag;                  // FIXME: to preservative
    using concrete_type      = clip_view<image_concrete_t<I>, D>;

    template <class V>
    using ch_value_type = clip_view<image_ch_value_t<I, V>, D>;

    static_assert(image_accessible_v<I>, "Image must be pw-accessible.");
    /// \}

  public:
    clip_view(I ima, D domain)
      : clip_view::image_adaptor{std::move(ima)}
      , m_domain{std::move(domain)}
    {
      // FIXME: possibly assert that domain is included in ima.domain()
    }

    template <class I2, class D2>
    clip_view(const clip_view<I2, D2>& other, mln::init)
      : clip_view::image_adaptor{static_cast<I>(other.base().template ch_value<value_type>())}
      , m_domain{other.m_domain}
    {
    }

    template <class I2, class D2>
    clip_view(const clip_view<I2, D2>& other, const value_type& v)
      : clip_view::image_adaptor{static_cast<I>((other.base().template ch_value<value_type>()).init(v))}
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
      auto g = [this](point_type p) -> new_pixel_type { return this->base().new_pixel_at(p); };
      return mln::ranges::view::transform(this->m_domain, g);
    }


    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    reference operator[](image_index_t<dummy> i)
    {
      return this->base()[i];
    }
    /// \}


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(m_domain.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base()(p);
    }
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return this->base().at(p);
    }
    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(m_domain.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().new_pixel(p);
    }
    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return this->base().new_pixel_at(p);
    }
    /// \}


    /// IndexableAndAccessible-image related methods
    /// \{
    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> index_of_point(point_type p) const
    {
      mln_precondition(m_domain.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().index_of_point(p);
    }

    template <typename dummy = I>
    point_type point_at_index(std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> i) const
    {
      return this->base().point_at_index(i);
    }

    template <typename dummy = I>
    std::enable_if_t<(indexable::value && accessible::value), image_index_t<dummy>> delta_index(point_type p) const
    {
      return this->base().delta_index(p);
    }
    /// \}


    /// Raw-image related methods
    /// \{
    auto data() const           = delete;
    auto data()                 = delete;
    auto strides(int dim) const = delete;
    /// \}
  };


  namespace view
  {
    // Clip customization point
    // if ima.clip(domain) exists and is well-formed, it is used
    // otherwise the generic morpher is used

    // Try first, if Substition succeeds, it is a best match
    template <class I, class D>
    auto clip(I&& ima, D&& domain) -> decltype(ima.clip(domain))
    {
      return ima.clip(std::forward<D>(domain));
    }


    // Used if the previous substition has failed
    template <class I, class D>
    clip_view<I, D> clip(const experimental::Image<I>& ima, D domain)
    {
      return {static_cast<const I&>(ima), std::forward<D>(domain)};
    }

  } // namespace view

} // namespace mln
