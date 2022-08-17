#pragma once

#include <ano/core/assert.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/image/view/adaptor.hpp>
#include <ano/core/range/view/transform.hpp>

#include <type_traits>

namespace ano
{


  template <class I, class F>
  class transform_view : public image_adaptor<I>, public ano::details::Image<transform_view<I, F>>
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
    using accessible = image_accessible_t<I>;
    using indexable  = image_indexable_t<I>;
    using view       = std::true_type;

    // The extension could be preserved if F returns a reference. Otherwise extension().fill() would
    // require to inverse F to fill with the good extension
    using extension_category = ano::extension::none_extension_tag;

    // Transform doesn't preserve contiguity, so it decays from raw_image_tag
    using category_type = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using concrete_type = image_ch_value_t<I, value_type>;

    template <typename V>
    using ch_value_type = image_ch_value_t<I, V>;
    /// \}

  public:
    /// Pixel type definitions
    /// \{
    struct pixel_type : pixel_adaptor<image_pixel_t<I>>, ano::details::Pixel<pixel_type>
    {
      using point_type               = transform_view::point_type;
      using site_type [[deprecated]] = transform_view::point_type;
      using reference                = transform_view::reference;
      using value_type               = transform_view::value_type;

      pixel_type(fun_t fun, image_pixel_t<I> px)
        : pixel_type::pixel_adaptor{px}
        , fun_{fun}
      {
      }

      pixel_type(const pixel_type& other)
        : pixel_type::pixel_adaptor{other}
        , fun_(other.fun_)
      {
      }
      pixel_type(pixel_type&& other)
        : pixel_type::pixel_adaptor{std::move(other)}
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

    auto values() { return ano::ranges::view::transform(this->base().values(), fun_); }

    auto pixels()
    {
      auto pxwrapper = [fun = this->fun_](image_pixel_t<I> px) { return pixel_type{fun, std::move(px)}; };
      return ano::ranges::view::transform(this->base().pixels(), pxwrapper);
    }

    decltype(auto) concretize() const { return this->base().template ch_value<value_type>(); }

    template <typename Val>
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
      ano_precondition(this->domain().has(p));
      return std::invoke(fun_, this->base()(p));
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return std::invoke(fun_, this->base().at(p));
    }

    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel(point_type p)
    {
      ano_precondition(this->domain().has(p));
      return {fun_, this->base().pixel(p)};
    }

    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel_at(point_type p)
    {
      return {fun_, this->base().pixel_at(p)};
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
  class transform2_view : public ano::details::Image<transform2_view<I1, I2, F>>
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
    using extension_category = ano::extension::none_extension_tag; // Preservative behavior (may be too preservative)
    // Transform doesn't preserve contiguity, so it decays from raw_image_tag
    using category_type =
        std::conditional_t<std::is_base_of_v<raw_image_tag, common_category>, bidirectional_image_tag, common_category>;
    using concrete_type = std::common_type_t<image_ch_value_t<I1, value_type>, image_ch_value_t<I2, value_type>>;

    template <typename V>
    using ch_value_type = std::common_type_t<image_ch_value_t<I1, V>, image_ch_value_t<I2, V>>;
    /// \}

  public:
    /// Pixel type definitions
    /// \{
    struct pixel_type : ano::details::Pixel<pixel_type>
    {
    public:
      using point_type               = transform2_view::point_type;
      using site_type [[deprecated]] = transform2_view::point_type;
      using reference                = transform2_view::reference;
      using value_type               = transform2_view::value_type;


      pixel_type(fun_t fun, image_pixel_t<I1> px1, image_pixel_t<I2> px2)
        : m_pix1{std::move(px1)}
        , m_pix2{std::move(px2)}
        , fun_{std::move(fun)}
      {
      }

      pixel_type(const pixel_type& other) = default;
      pixel_type(pixel_type&& other)      = default;

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

    auto values() { return ano::ranges::view::transform(m_ima1.values(), m_ima2.values(), fun_); }

    auto pixels()
    {
      auto pxwrapper = [fun = this->fun_](image_pixel_t<I1> px1, image_pixel_t<I2> px2) {
        return pixel_type{fun, std::move(px1), std::move(px2)};
      };
      return ano::ranges::view::transform(m_ima1.pixels(), m_ima2.pixels(), pxwrapper);
    }


    /// Accessible-image related methods
    /// \{
    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> operator()(point_type p)
    {
      ano_precondition(m_ima1.domain().has(p));
      ano_precondition(m_ima2.domain().has(p));
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename Ret = reference>
    std::enable_if_t<accessible::value, Ret> at(point_type p)
    {
      return std::invoke(fun_, m_ima1.at(p), m_ima2.at(p));
    }

    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel(point_type p)
    {
      ano_precondition(m_ima1.domain().has(p));
      ano_precondition(m_ima2.domain().has(p));
      return {fun_, m_ima1.pixel(p), m_ima2.pixel(p)};
    }

    template <typename Ret = pixel_type>
    std::enable_if_t<accessible::value, Ret> pixel_at(point_type p)
    {
      return {fun_, m_ima1.pixel_at(p), m_ima2.pixel_at(p)};
    }
    /// \}
  };


  namespace view
  {
    template <class I, class UnaryFunction>
    transform_view<I, UnaryFunction> transform(I ima, UnaryFunction fun)
    {
      static_assert(ano::is_a<I, ano::details::Image>());

      return {std::move(ima), std::move(fun)};
    }


    template <class I1, class I2, class UnaryFunction>
    transform2_view<I1, I2, UnaryFunction> transform(I1 ima1, I2 ima2, UnaryFunction fun)
    {
      static_assert(ano::is_a<I1, ano::details::Image>());
      static_assert(ano::is_a<I2, ano::details::Image>());
      static_assert(ano::concepts::InputImage<I1>);
      static_assert(ano::concepts::InputImage<I2>);

      return {std::move(ima1), std::move(ima2), std::move(fun)};
    }
  } // namespace view

} // namespace ano
