#pragma once

#include <mln/core/algorithm/paste.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/view/clamp_extended.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/mirror_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/periodize_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/se/view/filter.hpp>
#include <mln/core/trace.hpp>


#include <any>
#include <exception>
#include <limits>
#include <type_traits>
#include <concepts/type_traits.hpp>


namespace mln::extension
{
  template <typename Ima, typename SE>
  constexpr bool fit(const Ima& ima, const SE& se)
  {
    // TODO static_assert
    return ima.extension().fit(se);
  }

  template <typename Ext>
  constexpr bool is_finite(const Ext& ext)
  {
    return not(ext.extent() == std::numeric_limits<int>::max());
  }

  enum class BorderManagementMethod
  {
    None,
    Fill,
    Mirror,
    Periodize,
    Clamp,
    Image,
    User
  };

  enum class BorderManagementPolicy
  {
    Native,
    Auto
  };

  namespace detail
  {
    template <BorderManagementPolicy Policy, typename V = void>
    struct managed_image_type_by_policy;

    template <typename V>
    struct managed_image_type_by_policy<BorderManagementPolicy::Native, V>
    {
      template <typename I>
      using managed_image_t = std::variant<::concepts::remove_cvref_t<I>>;
    };

    template <typename V>
    struct managed_image_type_by_policy<BorderManagementPolicy::Auto, V>
    {
      template <typename I>
      using managed_image_t = std::variant<::concepts::remove_cvref_t<I>, ::concepts::remove_cvref_t<V>>;
    };


    template <typename Dom>
    struct adapt_se_none_border
    {
      explicit adapt_se_none_border(Dom dom)
        : m_dom{std::move(dom)}
      {
      }

      template <typename Pnt>
      auto operator()(Pnt&& pnt) const
      {
        return m_dom.has(std::forward<Pnt>(pnt));
      }

    private:
      Dom m_dom;
    };

    template <BorderManagementMethod Method, typename V = void>
    struct managed_structuring_element_by_method
    {
      template <typename SE, typename>
      using managed_se_t = std::variant<::concepts::remove_cvref_t<SE>>;
    };

    template <typename S>
    struct managed_structuring_element_by_method<BorderManagementMethod::None, S>
    {
      template <typename SE, typename Dom>
      using managed_se_t = std::variant<::concepts::remove_cvref_t<SE>, se_filter_view<adapt_se_none_border<Dom>, SE>>;
    };
  } // namespace detail

  template <BorderManagementMethod Method, BorderManagementPolicy Policy, typename I, typename SE, typename Dom,
            typename VI = void, typename VSE = void>
  struct managed_result
  {
    using type =
        std::pair<typename detail::managed_image_type_by_policy<Policy, VI>::template managed_image_t<I>,
                  typename detail::managed_structuring_element_by_method<Method, VSE>::template managed_se_t<SE, Dom>>;
  };
  template <BorderManagementMethod Method, BorderManagementPolicy Policy, typename I, typename SE, typename Dom,
            typename VI = void, typename VSE = void>
  using managed_result_t = typename managed_result<Method, Policy, I, SE, Dom, VI, VSE>::type;


  template <BorderManagementMethod Method, BorderManagementPolicy Policy = BorderManagementPolicy::Auto,
            typename U = void>
  class border_manager;


  //////////
  // NONE //
  //////////
  template <typename U>
  class border_manager<BorderManagementMethod::None, BorderManagementPolicy::Auto, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::None, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      // We remove the border usage by adapting the SE
      return {ima, view::se_filter(se, detail::adapt_se_none_border{ima.domain()})};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::None, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::None, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      static_assert(!image_has_extension<I>::value,
                    "The image has an extension when none is explicitly requested."
                    "Use an image without a border or switch to the auto border management policy!");

      // We remove the border usage by adapting the SE
      return {ima, view::se_filter(se, detail::adapt_se_none_border{ima.domain()})};
    }
  };


  //////////
  // FILL //
  //////////

  template <typename U>
  class border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::Auto, U>
  {
  private:
    std::any m_value;

  public:
    constexpr BorderManagementMethod method() const { return BorderManagementMethod::Fill; }

    template <class T>
    explicit border_manager(T value)
      : m_value{std::move(value)}
    {
    }

    std::any& get_value() { return m_value; }
    const std::any& get_value() const { return m_value; }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Fill, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            value_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      // using S = ::concepts::remove_cvref_t<SE>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<S, mln::StructuringElement>::value);

      auto* val = std::any_cast<image_value_t<I>>(&m_value);
      if (!val)
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      if constexpr (image_has_extension<I>::value && image_extension_t<I>::support_fill::value)
      {
        if (ima.extension().is_fill_supported() && fit(ima, se))
        {
          // The SE fits, we set the value of the border
          ima.extension().fill(*val);
          return {ima, se};
        }
      }
      mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension does "
                       "not really support fill. Consider using a large border.");

      return {view::value_extended(ima, *val), se};
    }


    template <class InputImage, class SE, class D>
    auto create_temporary_image(InputImage&& ima, const SE& se, const D& roi) const -> image_concrete_t<::concepts::remove_cvref_t<InputImage>>
    {
      using I = ::concepts::remove_cvref_t<InputImage>;

      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(mln::is_a<SE, mln::details::StructuringElement>());

      if (m_value.type() != typeid(image_value_t<I>))
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      D input_roi = se.compute_input_region(ima.domain());

      image_build_params params;
      params.init_value = m_value;

      image_concrete_t<I> output(input_roi, params);
      mln::paste(ima, roi, output);
      return output;
    }

    template <class I, class SE, class D>
    I create_temporary_image(const SE& se, const D& roi) const
    {
      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(mln::is_a<SE, mln::details::StructuringElement>());

      if (m_value.type() != typeid(image_value_t<I>))
        throw std::runtime_error(
          "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      D input_roi = se.compute_input_region(roi);

      image_build_params params;
      params.init_value = m_value;

      image_concrete_t<I> output(input_roi, params);
      return output;
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::Native, U>
  {
  private:
    std::any m_value;

  public:
    template <class T>
    explicit border_manager(T value)
      : m_value{std::move(value)}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Fill, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            value_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      auto* val = std::any_cast<image_value_t<I>>(&m_value);
      if (!val)
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_fill::value,
                    "The image's extension does not support the fill method."
                    "Use an image that does or switch to the auto border management policy!");

      if (!fit(ima, se) || !ima.extension().is_fill_supported())
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      ima.extension().fill(*val);
      return {ima, se};
    }
  };


  ////////////
  // MIRROR //
  ////////////

  template <typename U>
  class border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Auto, U>
  {
  public:

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Mirror, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            mirror_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      if constexpr (image_has_extension<I>::value && image_extension_t<I>::support_mirror::value)
      {
        if (ima.extension().is_mirror_supported() && fit(ima, se))
        {
          ima.extension().mirror();
          return {ima, se};
        }
      }

      mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension does "
                       "not really support mirror. Consider using a large border.");
      return {view::mirror_extended(ima), se};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Mirror, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            mirror_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_mirror::value,
                    "The image's extension does not support the mirror method."
                    "Use an image that does or switch to the auto border management policy!");

      if (!fit(ima, se) || !ima.extension().is_mirror_supported())
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fits and the extension dynamically support mirror, we set the value of the border
      ima.extension().mirror();
      return {ima, se};
    }
  };


  ///////////////
  // PERIODIZE //
  ///////////////

  template <typename U>
  class border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Auto, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Periodize, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            periodize_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      if constexpr (image_has_extension<I>::value && image_extension_t<I>::support_periodize::value)
      {
        if (ima.extension().is_periodize_supported() && fit(ima, se))
        {
          ima.extension().periodize();
          return {ima, se};
        }
      }

      mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension does "
                       "not really support periodize. Consider using a large border.");
      return {view::periodize_extended(ima), se};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Periodize, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            periodize_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_periodize::value,
                    "The image's extension does not support the periodize method."
                    "Use an image that does or switch to the auto border management policy!");

      if (!fit(ima, se) || !ima.extension().is_periodize_supported())
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fits and the extension dynamically support periodize, we set the value of the border
      ima.extension().periodize();
      return {ima, se};
    }
  };


  ///////////
  // CLAMP //
  ///////////

  template <typename U>
  class border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Auto, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Clamp, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            clamp_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      if constexpr (image_has_extension<I>::value && image_extension_t<I>::support_clamp::value)
      {
        if (ima.extension().is_clamp_supported() && fit(ima, se))
        {
          ima.extension().clamp();
          return {ima, se};
        }
      }

      mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension does "
                       "not really support clamp. Consider using a large border.");
      return {view::clamp_extended(ima), se};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Clamp, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            clamp_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_clamp::value,
                    "The image's extension does not support the clamp method."
                    "Use an image that does or switch to the auto border management policy!");

      if (!fit(ima, se) || !ima.extension().is_clamp_supported())
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fits and the extension dynamically support clamp, we set the value of the border
      ima.extension().clamp();
      return {ima, se};
    }
  };


  ///////////
  // IMAGE //
  ///////////

  template <typename U>
  class border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Auto, U>
  {
  private:
    U                m_baseimage;
    image_point_t<U> m_offset;

  public:
    explicit border_manager(U baseimage, image_point_t<U> offset = image_point_t<U>{})
      : m_baseimage{std::move(baseimage)}
      , m_offset{std::move(offset)}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Image, BorderManagementPolicy::Auto, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            image_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<I>>);
      static_assert(std::is_convertible_v<image_point_t<I>, image_value_t<U>>);

      if constexpr (image_has_extension<I>::value && image_extension_t<I>::support_extend_with::value)
      {
        if (ima.extension.is_extend_with_supported() && fit(ima, se))
        {
          // FIXME: to implement
          // ima.extension().extend_with(m_baseimage, m_offset);
          return {ima, se};
        }
      }

      mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                       "does not really support extend_with. Consider using a large border.");
      return {view::image_extended(ima, m_baseimage, m_offset), se};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Native, U>

  {
  private:
    U                m_baseimage;
    image_point_t<U> m_offset;

  public:
    explicit border_manager(U baseimage, image_point_t<U> offset = image_point_t<U>{})
      : m_baseimage{std::move(baseimage)}
      , m_offset{std::move(offset)}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::Image, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>,
                            image_extended_view<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<I>>);
      static_assert(std::is_convertible_v<image_point_t<I>, image_value_t<U>>);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_extend_with::value,
                    "The image's extension does not support the extend_with filling method."
                    "Use an image that does or switch to the auto border management policy!");

      if (!fit(ima, se) || !ima.extension().is_extend_with_supported())
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fits and the extension dynamically support extend_with, we set the value of the border
      // FIXME: to implement
      // ima.extension().extend_with(m_baseimage, m_offset);
      return {ima, se};
    }
  };


  //////////
  // USER //
  //////////

  template <typename U>
  class border_manager<BorderManagementMethod::User, BorderManagementPolicy::Auto, U>
  {
  public:
    static constexpr BorderManagementMethod method() { return BorderManagementMethod::User; }

    static constexpr std::any get_value() { return {}; }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::User, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");

      if (!fit(ima, se))
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fit the image
      return {ima, se};
    }

    template <class InputImage, class SE, class D>
    auto create_temporary_image(InputImage&& ima, const SE& se, const D& roi) const -> image_concrete_t<::concepts::remove_cvref_t<InputImage>>
    {
      using I = ::concepts::remove_cvref_t<InputImage>;

      static_assert(mln::is_a<I, mln::details::Image>());
      static_assert(mln::is_a<SE, mln::details::StructuringElement>());


      if (!fit(ima, se))
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element.");
      }

      D input_roi = se.compute_input_region(roi);

      image_concrete_t<I> output(input_roi);
      mln::impl::paste_unsafe(ima, input_roi, output);
      return output;
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::User, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_result_t<BorderManagementMethod::User, BorderManagementPolicy::Native, Ima, SE,
                            image_domain_t<::concepts::remove_cvref_t<Ima>>>
    {
      using I = ::concepts::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::details::Image>::value);
      // static_assert(mln::is_a<SE, mln::details::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");

      if (!fit(ima, se))
      {
        throw std::runtime_error(
            "The given image's extension is not large enough to fit the given structuring element."
            "Use an image that has an extension large enough or switch to the auto border management policy!");
      }

      // The SE fit the image
      return {ima, se};
    }
  };


  namespace bm
  {
    // Default policy
    inline namespace auto_
    {
      using none      = border_manager<BorderManagementMethod::None, BorderManagementPolicy::Auto>;
      using fill      = border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::Auto>;
      using mirror    = border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Auto>;
      using periodize = border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Auto>;
      using clamp     = border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Auto>;
      template <typename U>
      using image = border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Auto, U>;
      using user  = border_manager<BorderManagementMethod::User, BorderManagementPolicy::Auto>;
    } // namespace auto_

    namespace native
    {
      using none      = border_manager<BorderManagementMethod::None, BorderManagementPolicy::Native>;
      using fill      = border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::Native>;
      using mirror    = border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Native>;
      using periodize = border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Native>;
      using clamp     = border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Native>;
      template <typename U>
      using image = border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Native, U>;
      using user  = border_manager<BorderManagementMethod::User, BorderManagementPolicy::Native>;
    } // namespace native
  }   // namespace bm


} // namespace mln::extension
