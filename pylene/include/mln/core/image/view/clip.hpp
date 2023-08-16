#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/range/view/filter.hpp>
#include <mln/core/range/view/remove_if.hpp>
#include <mln/core/range/view/transform.hpp>

#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/range/primitives.hpp>

#include <type_traits>

namespace mln
{

  template <class I, class D>
  class clip_view : public image_adaptor<I>, public mln::details::Image<clip_view<I, D>>
  {
    D m_subdomain;

    template <class I2, class D2>
    friend class clip_view;

  public:
    /// Type definitions
    /// \{
    using typename clip_view::image_adaptor::pixel_type;
    using typename clip_view::image_adaptor::point_type;
    using typename clip_view::image_adaptor::reference;
    using typename clip_view::image_adaptor::value_type;
    using domain_type = D;
    /// \}

    static_assert(std::is_convertible_v<mln::ranges::mdrange_value_t<D>, point_type>,
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
    clip_view(I ima, D subdomain)
      : clip_view::image_adaptor{std::move(ima)}
      , m_subdomain{std::move(subdomain)}
    {
      // mln_precondition(::ranges::includes(m_ima.domain(), m_subdomain)); //FIXME
    }

    template <class I2, class D2>
    clip_view(const clip_view<I2, D2>& other, const image_build_params& params)
      : clip_view::image_adaptor{imchvalue<value_type>(other.base()).set_params(params).build()}
      , m_subdomain{other.m_subdomain}
    {
    }

    image_builder<concrete_type, clip_view> concretize() const { return {*this}; }

    template <class U>
    image_builder<ch_value_type<U>, clip_view> ch_value() const
    {
      return {*this};
    }

    domain_type domain() const { return m_subdomain; }

    auto values()
    {
      auto g = [this](point_type p) -> reference { return this->base().at(p); };
      return mln::ranges::view::transform(m_subdomain, g);
    }

    auto pixels()
    {
      auto g = [this](point_type p) -> pixel_type { return this->base().pixel_at(p); };
      return mln::ranges::view::transform(m_subdomain, g);
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
    reference operator()(point_type p) requires(accessible::value)
    {
      mln_precondition(domain().has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base()(p);
    }

    reference at(point_type p) requires(accessible::value) { return this->base().at(p); }

    pixel_type pixel(point_type p) requires(accessible::value)
    {
      mln_precondition(domain().has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().pixel(p);
    }
    pixel_type pixel_at(point_type p) requires(accessible::value) { return this->base().pixel_at(p); }
    /// \}


    /// IndexableAndAccessible-image related methods
    /// \{
    template <typename dummy = I>
    image_index_t<dummy> index_of_point(point_type p) const requires(indexable::value&& accessible::value)
    {
      return this->base().index_of_point(p);
    }

    template <typename dummy = I>
    point_type point_at_index(image_index_t<dummy> i) const requires(indexable::value&& accessible::value)
    {
      return this->base().point_at_index(i);
    }

    template <typename dummy = I>
    image_index_t<dummy> delta_index(point_type p) const requires(indexable::value&& accessible::value)
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
    clip_view<I, D> clip(const mln::details::Image<I>& ima, D domain)
    {
      return {static_cast<const I&>(ima), domain};
    }

  } // namespace view

} // namespace mln
