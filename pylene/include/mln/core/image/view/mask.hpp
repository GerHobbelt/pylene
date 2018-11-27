#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>
#include <mln/core/rangev3/view/filter.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <type_traits>

namespace mln
{

  template <class I, class M>
  class mask_view : public image_adaptor<I>, public New_Image<mask_view<I, M>>
  {
    M m_mask;

  public:
    /// Type definitions
    /// \{
    using typename mask_view::image_adaptor::point_type;
    using typename mask_view::image_adaptor::value_type;
    using typename mask_view::image_adaptor::new_pixel_type;
    using typename mask_view::image_adaptor::reference;
    //using domain_type    = where_t<M>;
    using concrete_type  = mask_view<concrete_t<I>, M>;

    template <class V>
    using ch_value_type  = mask_view<ch_value_t<I, V>, M>;
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible            = typename I::accessible;
    using indexable             = typename I::indexable;
    using extension_category    = mln::extension::none_extension_tag; // FIXME: should be improved
    /// \}


    // Concepts check
    static_assert(std::is_same_v<typename M::value_type, bool>,
                  "Image value type must be boolean.");

    static_assert(I::accessible::value,
                  "Image must be pw-accessible.");


  public:

    mask_view(I ima, M mask)
      : mask_view::image_adaptor{std::move(ima)}
      , m_mask{std::move(mask)}
    {
      // FIXME: possibly assert that mask.domain() == ima.domain()
    }

    template <class I2, class D2>
    mask_view(const mask_view<I2, D2>& other, mln::init)
      : mask_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()))}
      , m_mask{other.m_mask}
    {
    }

    template <class I2, class D2>
    mask_view(const mask_view<I2, D2>& other, const value_type& v)
      : mask_view::image_adaptor{static_cast<I>(imchvalue<value_type>(other.base()).init(v))}
      , m_mask{other.m_mask}
    {
    }

    internal::initializer<concrete_type, mask_view> concretize() const
    {
      return { *this };
    }

    template <class U>
    internal::initializer<ch_value_type<U>, mask_view> ch_value() const
    {
      return { *this };
    }

    //domain_type domain() const { return where(m_mask); }

    auto domain() {
      return mln::ranges::view::transform(
        mln::ranges::view::filter(m_mask.new_pixels(), [](const auto& px) { return px.val(); }),
        [] (const auto& px) { return px.point(); });
    }

    auto new_values()
    {
      auto zz = mln::ranges::view::zip(this->base().new_values(), m_mask.new_values());
      return mln::ranges::view::transform(
        mln::ranges::view::filter(zz, [](auto vtuple) { return std::get<1>(vtuple); }),
        [](auto vtuple) -> decltype(auto) { return std::get<0>(vtuple); });
    }

    auto new_pixels()
    {
      auto zz = mln::ranges::view::zip(this->base().new_pixels(), m_mask.new_values());
      return mln::ranges::view::transform(
        mln::ranges::view::filter(zz, [](auto vtuple) { return std::get<1>(vtuple); }),
        [](auto vtuple) -> new_pixel_type { return { std::get<0>(vtuple) }; });
    }

    reference operator()(point_type p)
    {
      mln_precondition(m_mask(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().at(p);
    }

    reference at(point_type p)
    {
      return this->base().at(p);
    }


    new_pixel_type new_pixel(point_type p)
    {
      mln_precondition(m_mask.has(p));
      mln_precondition(this->base().domain().has(p));
      return this->mask_view::image_adaptor::new_pixel_at(p);
    }

    new_pixel_type new_pixel_at(point_type p)
    {
      return this->mask_view::image_adaptor::new_pixel_at(p);
    }
  };


  namespace view
  {
    template <class I, class M>
    mask_view<I, M> mask(I ima, M mask)
    {
      static_assert(mln::is_a<I, New_Image>());
      static_assert(mln::is_a<M, New_Image>());

      // FIXME: make ima a view first ?
      return { std::move(ima), std::move(mask) };
    }

  } // namespace view

}
