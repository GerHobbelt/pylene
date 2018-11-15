#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>
#include <type_traits>

namespace mln
{


  template <class I, class F>
  class transform_view : public image_adaptor<I>
  {
    using fun_t = F; // FIXME something with semiregular_t<F> ?
    fun_t f;

  public:
    /// Type definitions
    /// \{
    using reference                      = std::invoke_result_t<F&, typename I::reference>;
    using value_type                     = std::decay_t<reference>;
    using point_type                     = typename I::point_type;

    using concrete_type  = ch_value_t<I, value_type>;

    template <class V>
    using ch_value_type = ch_value_t<I, V>;

    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible     = typename I::accessible;
    using indexable      = typename I::indexable;
    using extension_category = mln::extension::none_extension_tag; // FIXME: should be improved
    /// \}

  private:
    // FIXME may be a simpler way ?
    // note that pixels() can return a proxy convertible to the pixel_type ? Or does it?
    // using pixel_range_base_type = decltype(std::declval<I&>.pixels());
    // using pixel_proxy_base_type = ::ranges::range_value_t<pixel_proxy_base_type>;

  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : pixel_adaptor<pixel_t<I>>, Pixel<new_pixel_type>
    {
      new_pixel_type() = default;
      new_pixel_type(F fun, pixel_t<I> px)
        : new_pixel_type::pixel_adaptor{px}
        , f{fun}
      {
      }

      decltype(auto) val() const { return std::invoke(f, this->base().val()); }

    private:
      fun_t f;
    };
    /// \}




    transform_view(I ima, F fun)
      : transform_view::image_adaptor{std::move(ima)}
      , f{std::move(fun)}
    {
    }

    decltype(auto) concretize() const { return imchvalue<value_type>(this->base()); }

    template <class U>
    decltype(auto) ch_value() const
    {
      return imchvalue<U>(this->base());
    }

    auto new_values() { return mln::ranges::view::transform(this->base().new_values(), f); }

    auto new_pixels()
    {
      using R = decltype(this->base().new_pixels());
      auto pxwrapper = [fun = this->f](::ranges::range_value_t<R> px) { return new_pixel_type{fun, std::move(px)}; };
      return mln::ranges::view::transform(this->base().new_pixels(), pxwrapper);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return std::invoke(f, this->base()(p));
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return std::invoke(f, this->base()(p));
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return {f, this->base().pixel(p)};
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return {f, this->base().pixel_at(p)};
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, reference> operator[](typename dummy::size_type i)
    {
      return std::invoke(f, this->base()[i]);
    }

  };


  namespace view
  {
    template <class Image, class UnaryFunction>
    transform_view<Image, UnaryFunction> transform(Image ima, UnaryFunction f)
    {
      // FIXME: make ima a view first ?
      return { std::move(ima), std::move(f) };
    }
  } // namespace view

} // namespace mln
