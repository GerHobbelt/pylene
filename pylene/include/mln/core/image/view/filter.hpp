#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/filter.hpp>
#include <type_traits>

namespace mln
{


  template <class I, class F>
  class filter_view : public image_adaptor<I>, public New_Image<filter_view<I, F>>
  {
    using fun_t = F; // FIXME something with semiregular_t<F> ?
    fun_t f;

  public:
    /// Type definitions
    /// \{
    using reference                      = std::invoke_result_t<F&, typename I::reference>;
    using value_type                     = std::decay_t<reference>;
    using point_type                     = typename I::point_type;

    using concrete_type  = void; // This image has no automatic concrete type

    template <class V>
    using ch_value_type = void;  // This image has no automatic concrete type

    using typename filter_view::image_adaptor::new_pixel_type;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible     = typename I::accessible;
    using indexable      = std::false_type;                             // May be too restrictive
    using extension_category = mln::extension::none_extension_tag;      // May be too restrictive (might be extended by image)
    /// \}


  private:

    struct pix_filter_fn
    {
      fun_t f;

      template <class Pix>
      bool operator() (const Pix& px) const
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

    auto domain() const
    {
      // FIXME: this range has not a "has" method so not a domain.

      auto g = [this](point_type p) -> bool { return this->f(this->base().at(p)); };
      return mln::ranges::view::filter(this->base().domain(), g);
    }

    //using domain_type = decltype(domain());

    auto new_values() { return mln::ranges::view::filter(this->base().new_values(), f); }

    auto new_pixels()
    {
      auto g = [f_ = this->f](const auto& px) -> bool { return f_(px.val()); };
      return mln::ranges::view::filter(this->base().new_pixels(), g);
    }


    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      mln_precondition(std::invoke(f, this->base().at(p)));
      return this->base().at(p);
    }


    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      return this->base().at(p);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      mln_precondition(std::invoke(f, this->base().at(p)));

      return this->base().new_pixel(p);
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return this->base().new_pixel_at(p);
    }
  };


  namespace view
  {
    template <class I, class P>
    filter_view<I, P> filter(I ima, P predicate)
    {
      static_assert(mln::is_a<I, New_Image>());
      static_assert(std::is_invocable_r<bool, P, image_reference_t<I>>());

      return { std::move(ima), std::move(predicate) };
    }
  }

  
  
}
