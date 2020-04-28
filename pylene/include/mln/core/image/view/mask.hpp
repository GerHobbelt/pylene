#pragma once

#include <mln/core/domain/where.hpp>
#include <mln/core/range/view/transform_if.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>




#include <type_traits>


namespace mln
{
  namespace details
  {
    struct first_of_two
    {
      template <class A1, class A2>
      A1 operator()(A1&& arg1, A2&&) const { return arg1; }
    };

    struct second_of_two
    {
      template <class A1, class A2>
      A2 operator()(A1&&, A2&& arg2) const { return arg2; }
    };
  } // namespace details


  template <class I, class M>
  class mask_view : public image_adaptor<I>, public experimental::Image<mask_view<I, M>>
  {
    M m_mask;

  public:
    /// Type definitions
    /// \{
    using new_pixel_type = image_pixel_t<I>;
    using typename mask_view::image_adaptor::point_type;
    using typename mask_view::image_adaptor::reference;
    using typename mask_view::image_adaptor::value_type;
    using domain_type = mln::ranges::where_t<M, ::ranges::identity>;
    /// \}

    static_assert(std::is_same_v<typename M::value_type, bool>, "Image value type must be boolean.");
    static_assert(image_accessible_v<M>, "The mask must accessible (ima(p) must be valid");


    /// Traits & Image Properties
    /// \{
    using category           = forward_image_tag;
    using accessible         = image_accessible_t<I>;
    using indexable          = image_indexable_t<I>;
    using view               = std::true_type;
    using extension_category = mln::extension::none_extension_tag; // FIXME: should be improved
    using concrete_type      = mask_view<image_concrete_t<I>, M>;

    template <class V>
    using ch_value_type = mask_view<image_ch_value_t<I, V>, M>;
    /// \}


  public:
    mask_view(I ima, M mask)
      : mask_view::image_adaptor{std::move(ima)}
      , m_mask{std::move(mask)}
    {
      // FIXME: possibly assert that mask.domain() == ima.domain()
    }


    template <class I2, class D2>
    mask_view(const mask_view<I2, D2>& other, const image_build_params& params)
      : mask_view::image_adaptor{ imchvalue<value_type>(other.base()).set_params(params).build() }
      , m_mask{other.m_mask}
    {
    }


    image_builder<concrete_type, mask_view> concretize() const { return {*this}; }

    template <class U>
    image_builder<ch_value_type<U>, mask_view> ch_value() const { return {*this}; }

    domain_type domain() const { return mln::experimental::where(m_mask); }

    auto new_values()
    {
      return mln::ranges::view::transform_if(details::first_of_two{}, details::second_of_two{}, this->base().new_values(), m_mask.new_values());
    }

    auto new_pixels()
    {
      return mln::ranges::view::transform_if(details::first_of_two{}, details::second_of_two{}, this->base().new_pixels(), m_mask.new_values());
    }

    /// Accessible-image related methods (overwritten from adaptor)
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(m_mask.domain().has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base()(p);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(m_mask.domain().has(p));
      mln_precondition(this->base().domain().has(p));
      return this->base().new_pixel(p);
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return this->base().new_pixel_at(p);
    }
    /// \}


    /// Raw-image related methods
    /// \{
    auto data() const       = delete;
    auto data()             = delete;
    auto strides(int) const = delete;
    /// \}
  };


  namespace view
  {
    template <class I, class M>
    mask_view<I, M> mask(I ima, M mask)
    {
      static_assert(mln::is_a<I, experimental::Image>());
      static_assert(mln::is_a<M, experimental::Image>());

      // FIXME: make ima a view first ?
      return {std::move(ima), std::move(mask)};
    }

  } // namespace view

} // namespace mln
