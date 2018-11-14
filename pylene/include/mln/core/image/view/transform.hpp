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
    using const_reference [[deprecated]] = std::invoke_result_t<const F&, typename I::const_reference>;
    using value_type                     = std::decay_t<const_reference>;
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


    struct new_const_pixel_type : pixel_adaptor<pixel_t<const I>>, Pixel<new_const_pixel_type>
    {
      new_const_pixel_type() = default;
      new_const_pixel_type(F fun, pixel_t<const I> px)
        : new_const_pixel_type::pixel_adaptor{px}
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

    auto values() const { return ranges::view::transform(f, this->base().values()); }
    auto values() { return ranges::view::transform(f, this->base().values()); }

    auto pixels() const
    {
      auto pxwrapper = [fun = this->f](pixel_t<I> px) { return new_pixel_type{fun, std::move(px)}; };
      return ranges::view::transform(this->base().pixels(), pxwrapper);
    }

    auto pixels()
    {
      auto pxwrapper = [fun = this->f](pixel_t<I> px) { return new_pixel_type{fun, std::move(px)}; };
      return ranges::view::transform(this->base().pixels(), pxwrapper);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return std::invoke(f, this->base()(p));
    }

    template <typename dummy = const_reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p) const
    {
      mln_precondition(this->domain().has(p));
      return std::invoke(f, this->base()(p));
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return std::invoke(f, this->base()(p));
    }

    template <typename dummy = const_reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p) const
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

    template <typename dummy = new_const_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p) const
    {
      mln_precondition(this->base().domain().has(p));
      return {f, this->base().pixel(p)};
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return {f, this->base().pixel_at(p)};
    }

    template <typename dummy = new_const_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p) const
    {
      return {f, this->base().pixel_at(p)};
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, reference> operator[](typename dummy::size_type i)
    {
      return std::invoke(f, this->base()[i]);
    }

    template <typename dummy = I>
    std::enable_if_t<indexable::value, const_reference> operator[](typename dummy::size_type i) const
    {
      return std::invoke(f, this->base()[i]);
    }

  };


  namespace view
  {
    template <class Image, class UnaryFunction>
    transform_view<Image, UnaryFunction> transform(Image ima, UnaryFunction f)
    {
      return { std::move(ima), std::move(f) };
    }
  } // namespace view

} // namespace mln
