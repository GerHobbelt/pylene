#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>
#include <type_traits>

namespace mln
{


  template <class I, class F>
  class transform_view : public image_adaptor<I>, public New_Image<transform_view<I, F>>
  {
    using fun_t = F;
    fun_t fun_;

  public:
    /// Type definitions
    /// \{
    using reference  = std::invoke_result_t<F&, typename I::reference>;
    using value_type = std::decay_t<reference>;
    using point_type = typename I::point_type;

    static_assert(!(std::is_rvalue_reference_v<reference> && !std::is_reference_v<typename I::reference>),
                  "The transformed image returns a temporary and the mapping function is a projection.\n"
                  "This is building a dangling reference.");

    using concrete_type = ch_value_t<I, value_type>;

    template <class V>
    using ch_value_type = ch_value_t<I, V>;

    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = typename I::accessible;
    using indexable          = typename I::indexable;
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
    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, Pixel<new_pixel_type>
    {
      using reference  = transform_view::reference;
      using value_type = transform_view::value_type;

      new_pixel_type(fun_t fun, image_pixel_t<I> px)
        : new_pixel_type::pixel_adaptor{px}
        , fun_{fun}
      {
      }

      new_pixel_type(const new_pixel_type& other)
        : new_pixel_type::pixel_adaptor{other}
        , fun_(other.fun_)
      {
      }
      new_pixel_type(new_pixel_type&& other)
        : new_pixel_type::pixel_adaptor{std::move(other)}
        , fun_(std::move(other.fun_))
      {
      }
      new_pixel_type& operator=(const new_pixel_type& other)
      {
        if (this != &other)
        {
          new_pixel_type::pixel_adaptor::operator=(other);

          fun_ = other.fun_;
        }

        return *this;
      }
      new_pixel_type& operator=(new_pixel_type&& other)
      {
        if (this != &other)
        {
          new_pixel_type::pixel_adaptor::operator=(std::move(other));

          fun_ = std::move(other.fun_);
        }

        return *this;
      }

      reference val() const { return std::invoke(fun_, this->base().val()); }

    private:
      fun_t fun_;
    };
    /// \}


    transform_view(I ima, F fun)
      : transform_view::image_adaptor{std::move(ima)}
      , fun_{std::move(fun)}
    {
    }

    transform_view(const transform_view& other)
      : transform_view::image_adaptor{other}
      , fun_(other.fun_)
    {
    }
    transform_view(transform_view&& other)
      : transform_view::image_adaptor{std::move(other)}
      , fun_(std::move(other.fun_))
    {
    }
    transform_view& operator=(const transform_view& other)
    {
      if (this != &other)
      {
        transform_view::image_adaptor::operator=(other);

        fun_ = other.fun_;
      }

      return *this;
    }
    transform_view& operator=(transform_view&& other)
    {
      if (this != &other)
      {
        transform_view::image_adaptor::operator=(std::move(other));

        fun_ = std::move(other.fun_);
      }

      return *this;
    }


    decltype(auto) concretize() const { return imchvalue<value_type>(this->base()); }

    template <class U>
    decltype(auto) ch_value() const
    {
      return imchvalue<U>(this->base());
    }

    auto new_values() { return mln::ranges::view::transform(this->base().new_values(), fun_); }

    auto new_pixels()
    {
      using R        = decltype(this->base().new_pixels());
      auto pxwrapper = [fun = this->fun_](::ranges::range_value_t<R> px) { return new_pixel_type{fun, std::move(px)}; };
      return mln::ranges::view::transform(this->base().new_pixels(), pxwrapper);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return std::invoke(fun_, this->base()(p));
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      return std::invoke(fun_, this->base().at(p));
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      mln_precondition(this->base().domain().has(p));
      return {fun_, this->base().new_pixel(p)};
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return {fun_, this->base().new_pixel_at(p)};
    }

    template <typename dummy = I>
    reference operator[](typename dummy::size_type i)
    {
      return std::invoke(fun_, this->base()[i]);
    }
  };

  template <class I1, class I2, class F>
  class transform2_view : public New_Image<transform2_view<I1, I2, F>>
  {
    using fun_t = F; // FIXME something with semiregular_t<F> ?
    I1    m_ima1;
    I2    m_ima2;
    fun_t fun_;

  public:
    /// Type definitions
    /// \{
    using reference   = std::invoke_result_t<F&, typename I1::reference, typename I2::reference>;
    using value_type  = std::decay_t<reference>;
    using point_type  = typename I1::point_type;
    using domain_type = typename I1::domain_type;
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = std::bool_constant<I1::accessible::value && I2::accessible::value>;
    using indexable          = std::false_type;                    // Preservative behavior
    using extension_category = mln::extension::none_extension_tag; // Preservative behavior (may be too preservative)

    using concrete_type = ch_value_t<I1, value_type>;

    template <class V>
    using ch_value_type = ch_value_t<I1, V>;
    /// \}

  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : Pixel<new_pixel_type>
    {
    public:
      using point_type               = typename transform2_view::point_type;
      using site_type [[deprecated]] = point_type;
      using value_type               = typename transform2_view::value_type;
      using reference                = typename transform2_view::reference;


      new_pixel_type(fun_t fun, image_pixel_t<I1> px1, image_pixel_t<I2> px2)
        : m_pix1{std::move(px1)}
        , m_pix2{std::move(px2)}
        , fun_{std::move(fun)}
      {
      }

      // FIXME: implement copy/move ctors/operators

      reference val() const { return std::invoke(fun_, m_pix1.val(), m_pix2.val()); }
      auto      point() const { return m_pix1.point(); }
      void      advance(point_type p)
      {
        m_pix1.advance(p);
        m_pix2.advance(p);
      }

    private:
      fun_t             fun_;
      image_pixel_t<I1> m_pix1;
      image_pixel_t<I2> m_pix2;
    };
    /// \}

    transform2_view(I1 ima1, I2 ima2, F fun)
      : m_ima1{std::move(ima1)}
      , m_ima2{std::move(ima2)}
      , fun_{std::move(fun)}
    {
    }

    // FIXME: implement copy/move ctors/operators


    decltype(auto) concretize() const { return imchvalue<value_type>(this->base()); }

    template <class U>
    decltype(auto) ch_value() const
    {
      return imchvalue<U>(this->base());
    }

    auto domain() const { return m_ima1.domain(); }

    auto new_values() { return mln::ranges::view::transform(m_ima1.new_values(), m_ima2.new_values(), fun_); }

    auto new_pixels()
    {
      using R1       = decltype(m_ima1.new_pixels());
      using R2       = decltype(m_ima2.new_pixels());
      auto pxwrapper = [fun = this->fun_](::ranges::range_reference_t<R1> px1, ::ranges::range_reference_t<R2> px2) {
        return new_pixel_type{fun, std::move(px1), std::move(px2)};
      };
      return mln::ranges::view::transform(m_ima1.new_pixels(), m_ima2.new_pixels(), pxwrapper);
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> operator()(point_type p)
    {
      mln_precondition(m_ima1.domain().has(p));
      mln_precondition(m_ima2.domain().has(p));
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename dummy = reference>
    std::enable_if_t<accessible::value, dummy> at(point_type p)
    {
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel(point_type p)
    {
      mln_precondition(m_ima1.domain().has(p));
      mln_precondition(m_ima2.domain().has(p));
      return {fun_, m_ima1.new_pixel(p), m_ima2.new_pixel(p)};
    }

    template <typename dummy = new_pixel_type>
    std::enable_if_t<accessible::value, dummy> new_pixel_at(point_type p)
    {
      return {fun_, m_ima1.new_pixel_at(p), m_ima2.new_pixel_at(p)};
    }
  };


  namespace view
  {
    template <class I, class UnaryFunction>
    transform_view<I, UnaryFunction> transform(I ima, UnaryFunction fun)
    {
      static_assert(mln::is_a<I, New_Image>());


      // FIXME: make ima a view first ?
      return {std::move(ima), std::move(fun)};
    }


    template <class I1, class I2, class UnaryFunction>
    transform2_view<I1, I2, UnaryFunction> transform(I1 ima1, I2 ima2, UnaryFunction fun)
    {
      static_assert(mln::is_a<I1, New_Image>());
      static_assert(mln::is_a<I2, New_Image>());

      // FIXME: make ima1 and ima2 a view first ?
      return {std::move(ima1), std::move(ima2), std::move(fun)};
    }
  } // namespace view

} // namespace mln
