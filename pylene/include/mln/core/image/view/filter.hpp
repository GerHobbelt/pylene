#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/private/filtered.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/filter.hpp>

#include <range/v3/empty.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/functional.hpp>

#include <type_traits>


namespace mln
{

  template <class I, class F>
  class filter_view : public image_adaptor<I>, public experimental::Image<filter_view<I, F>>
  {
    using fun_t = F;
    fun_t f;

  public:
    /// Type definitions
    /// \{
    using reference  = std::invoke_result_t<F&, image_reference_t<I>>;
    using value_type = std::decay_t<reference>;
    using typename filter_view::image_adaptor::new_pixel_type;
    using typename filter_view::image_adaptor::point_type;
    using domain_type = detail::filtered<I, F>;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible = image_accessible_t<I>;
    using indexable  = std::false_type;
    using view       = std::true_type;
    // May be too restrictive (might be extended by image)
    using extension_category = mln::extension::none_extension_tag;
    using concrete_type      = void; // This image has no automatic concrete type
    template <class V>
    using ch_value_type = void; // This image has no automatic concrete type
    /// \}


  private:
    struct pix_filter_fn
    {
      fun_t f;

      template <class Pix>
      bool operator()(const Pix& px) const
      {
        return f(px.val());
      }
    };

  public:
    filter_view(I ima, F fun)
      : filter_view::image_adaptor{std::move(ima)}
      , f{std::move(fun)}
    {
    }

    domain_type domain() const { return detail::filtered{this->base(), this->f}; }


    auto new_values() { return mln::ranges::view::filter(this->base().new_values(), f); }

    auto new_pixels()
    {
      auto g = [f_ = this->f](const auto& px) -> bool { return f_(px.val()); };
      return mln::ranges::view::filter(this->base().new_pixels(), g);
    }


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      mln_precondition(std::invoke(f, this->base()(p)));
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
      mln_precondition(this->base().domain().has(p));
      mln_precondition(std::invoke(f, this->base().at(p)));
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
    auto data() const           = delete;
    auto data()                 = delete;
    auto strides(int dim) const = delete;
    /// \}
  };


  namespace view
  {
    template <class I, class P>
    filter_view<I, P> filter(I ima, P predicate)
    {
      static_assert(mln::is_a<I, experimental::Image>());
      static_assert(std::is_invocable_r<bool, P, image_reference_t<I>>());

      return {std::move(ima), std::move(predicate)};
    }
  } // namespace view

} // namespace mln
