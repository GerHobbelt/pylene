#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <type_traits>

namespace mln
{


  template <class I, class F>
  class transform_view : public image_adaptor<I>, public experimental::Image<transform_view<I, F>>
  {
    using fun_t = F;
    fun_t fun_;

  public:
    /// Type definitions
    /// \{
    using reference   = std::invoke_result_t<F&, image_reference_t<I>>;
    using value_type  = std::remove_cv_t<std::remove_reference_t<reference>>;
    using point_type  = image_point_t<I>;
    using domain_type = image_domain_t<I>;

    static_assert(!(std::is_rvalue_reference_v<reference> && !std::is_reference_v<image_reference_t<I>>),
                  "The transformed image returns a temporary and the mapping function is a projection.\n"
                  "This is building a dangling reference.");
    /// \}


    /// Traits & Image Properties
    /// \{
    using accessible         = image_accessible_t<I>;
    using indexable          = image_indexable_t<I>;
    using view               = std::true_type;
    using extension_category = image_extension_category_t<I>;
    // Transform doesn't preserve contiguity, so it decays from raw_image_tag
    using category_type = std::conditional_t<std::is_base_of_v<raw_image_tag, image_category_t<I>>,
                                             bidirectional_image_tag, image_category_t<I>>;
    using concrete_type = image_ch_value_t<I, value_type>;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value V>
#else
    template <typename V>
#endif
    using ch_value_type = image_ch_value_t<I, V>;
    /// \}

  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : pixel_adaptor<image_pixel_t<I>>, mln::experimental::Pixel<new_pixel_type>
    {
      using point_type              = transform_view::point_type;
      using site_type[[deprecated]] = transform_view::point_type;
      using reference               = transform_view::reference;
      using value_type              = transform_view::value_type;

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

      reference val() const { return std::invoke(fun_, this->base().val()); }

    private:
      fun_t fun_;
    };
    /// \}


    /////////////////
    // Image proxy //
    /////////////////

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

    auto new_values() { return mln::ranges::view::transform(this->base().new_values(), fun_); }

    auto new_pixels()
    {
      using R        = decltype(this->base().new_pixels());
      auto pxwrapper = [fun = this->fun_](::ranges::range_value_t<R> px) { return new_pixel_type{fun, std::move(px)}; };
      return mln::ranges::view::transform(this->base().new_pixels(), pxwrapper);
    }

    decltype(auto) concretize() const { return this->base().template ch_value<value_type>(); }

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value Val>
#else
    template <typename Val>
#endif
    decltype(auto) ch_value() const
    {
      return this->base().template ch_value<Val>();
    }


    /// Indexable-image related methods
    /// \{
    template <typename dummy = I>
    reference operator[](image_index_t<dummy> i)
    {
      return std::invoke(fun_, this->base()[i]);
    }
    /// \}


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(this->domain().has(p));
      return std::invoke(fun_, this->base()(p));
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return std::invoke(fun_, this->base().at(p));
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(this->domain().has(p));
      return {fun_, this->base().new_pixel(p)};
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return {fun_, this->base().new_pixel_at(p)};
    }
    /// \}


    /// Raw-image related methods
    /// \{
    auto data() const       = delete;
    auto data()             = delete;
    auto strides(int) const = delete;
    /// \}
  };

  template <class I1, class I2, class F>
  class transform2_view : public experimental::Image<transform2_view<I1, I2, F>>
  {
    using fun_t           = F;
    using common_category = std::common_type_t<image_category_t<I1>, image_category_t<I2>>;
    I1    m_ima1;
    I2    m_ima2;
    fun_t fun_;

  public:
    /// Type definitions
    /// \{
    using reference   = std::invoke_result_t<F&, typename I1::reference, typename I2::reference>;
    using value_type  = std::remove_cv_t<std::remove_reference_t<reference>>;
    using point_type  = std::common_type_t<image_point_t<I1>, image_point_t<I2>>;
    using domain_type = std::common_type_t<image_domain_t<I1>, image_domain_t<I2>>;
    /// \}

    /// Traits & Image Properties
    /// \{
    using accessible         = std::bool_constant<I1::accessible::value && I2::accessible::value>;
    using indexable          = std::false_type; // Preservative behavior
    using view               = std::true_type;
    using extension_category = mln::extension::none_extension_tag; // Preservative behavior (may be too preservative)
    // Transform doesn't preserve contiguity, so it decays from raw_image_tag
    using category_type =
        std::conditional_t<std::is_base_of_v<raw_image_tag, common_category>, bidirectional_image_tag, common_category>;
    using concrete_type = std::common_type_t<image_ch_value_t<I1, value_type>, image_ch_value_t<I2, value_type>>;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <concepts::Value V>
#else
    template <typename V>
#endif
    using ch_value_type = std::common_type_t<image_ch_value_t<I1, V>, image_ch_value_t<I2, V>>;
    /// \}

  public:
    /// Pixel type definitions
    /// \{
    struct new_pixel_type : mln::experimental::Pixel<new_pixel_type>
    {
    public:
      using point_type              = transform2_view::point_type;
      using site_type[[deprecated]] = transform2_view::point_type;
      using reference               = transform2_view::reference;
      using value_type              = transform2_view::value_type;


      new_pixel_type(fun_t fun, image_pixel_t<I1> px1, image_pixel_t<I2> px2)
        : m_pix1{std::move(px1)}
        , m_pix2{std::move(px2)}
        , fun_{std::move(fun)}
      {
      }

      new_pixel_type(const new_pixel_type& other) = default;
      new_pixel_type(new_pixel_type&& other)      = default;

      reference val() const { return std::invoke(fun_, m_pix1.val(), m_pix2.val()); }
      auto      point() const { return m_pix1.point(); }
      void      shift(point_type p)
      {
        m_pix1.shift(p);
        m_pix2.shift(p);
      }

    private:
      image_pixel_t<I1> m_pix1;
      image_pixel_t<I2> m_pix2;
      fun_t             fun_;
    };
    /// \}

    transform2_view(I1 ima1, I2 ima2, F fun)
      : m_ima1{std::move(ima1)}
      , m_ima2{std::move(ima2)}
      , fun_{std::move(fun)}
    {
    }

    transform2_view(const transform2_view& other) = default;
    transform2_view(transform2_view&& other)      = default;


    decltype(auto) concretize() const { return m_ima1.template ch_value<value_type>(); };

    template <class V>
    decltype(auto) ch_value() const
    {
      return m_ima1.template ch_value<V>();
    }

    auto domain() const { return m_ima1.domain(); }

    auto new_values()
    {
      static_assert(::ranges::ForwardRange<decltype(m_ima1.new_values())>());
      static_assert(::ranges::ForwardRange<decltype(m_ima2.new_values())>());

      return mln::ranges::view::transform(m_ima1.new_values(), m_ima2.new_values(), fun_);
    }

    auto new_pixels()
    {
      using R1 = decltype(m_ima1.new_pixels());
      using R2 = decltype(m_ima2.new_pixels());
      static_assert(::ranges::ForwardRange<R1>());
      static_assert(::ranges::ForwardRange<R2>());
      
      auto pxwrapper = [fun = this->fun_](::ranges::range_reference_t<R1> px1, ::ranges::range_reference_t<R2> px2) {
        return new_pixel_type{fun, std::move(px1), std::move(px2)};
      };
      return mln::ranges::view::transform(m_ima1.new_pixels(), m_ima2.new_pixels(), pxwrapper);
    }


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      mln_precondition(m_ima1.domain().has(p));
      mln_precondition(m_ima2.domain().has(p));
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel(point_type p)
    {
      mln_precondition(m_ima1.domain().has(p));
      mln_precondition(m_ima2.domain().has(p));
      return {fun_, m_ima1.new_pixel(p), m_ima2.new_pixel(p)};
    }

    template <typename Ret = new_pixel_type>
    std::enable_if_t<accessible::value, Ret> new_pixel_at(point_type p)
    {
      return {fun_, m_ima1.new_pixel_at(p), m_ima2.new_pixel_at(p)};
    }
    /// \}
  };


  namespace view
  {
    template <class I, class UnaryFunction>
    transform_view<I, UnaryFunction> transform(I ima, UnaryFunction fun)
    {
      static_assert(mln::is_a<I, experimental::Image>());

      return {std::move(ima), std::move(fun)};
    }


    template <class I1, class I2, class UnaryFunction>
    transform2_view<I1, I2, UnaryFunction> transform(I1 ima1, I2 ima2, UnaryFunction fun)
    {
      static_assert(mln::is_a<I1, experimental::Image>());
      static_assert(mln::is_a<I2, experimental::Image>());

#ifdef PYLENE_CONCEPT_TS_ENABLED
      static_assert(mln::concepts::InputImage<I1>);
      static_assert(mln::concepts::InputImage<I2>);
#endif

      return {std::move(ima1), std::move(ima2), std::move(fun)};
    }
  } // namespace view

} // namespace mln
