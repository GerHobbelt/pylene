#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/view/clamp_extended.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/mirror_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/periodize_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/trace.hpp>
#include <mln/core/utils/remove_cvref.hpp>

#include <any>
#include <exception>
#include <type_traits>


namespace mln::extension
{
  template <typename Ima, typename SE>
  constexpr bool fit(const Ima& ima, const SE& se)
  {
    // TODO static_assert
    return ima.extension().fit(se);
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

  template <BorderManagementPolicy Policy, typename V = void>
  struct managed_image_type_by_policy;

  template <typename V>
  struct managed_image_type_by_policy<BorderManagementPolicy::Native, V>
  {
    template <typename I>
    using managed_image_t = std::variant<mln::detail::remove_cvref_t<I>>;
  };

  template <typename V>
  struct managed_image_type_by_policy<BorderManagementPolicy::Auto, V>
  {
    template <typename I>
    using managed_image_t = std::variant<mln::detail::remove_cvref_t<I>, mln::detail::remove_cvref_t<V>>;
  };

  template <BorderManagementPolicy Policy, typename I, typename V>
  using managed_image_t = typename managed_image_type_by_policy<Policy, V>::template managed_image_t<I>;


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
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, none_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if (!fit(ima, se))
      {
        // The SE doesn't fit, no need to remove the border
        return {ima};
      }
      else
      {
        // The SE fits, we remove the border
        return {view::none_extended(ima)};
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::None, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE&) const
        -> managed_image_t<BorderManagementPolicy::Native, Ima, none_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      static_assert(!image_has_extension<I>::value,
                    "The image has an extension when none is explicitly requested."
                    "Use an image without a border or switch to the auto border management policy!");

      return {ima};
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
    template <class T>
    explicit border_manager(T value)
      : m_value{std::move(value)}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, value_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      // using S = mln::detail::remove_cvref_t<SE>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<S, mln::experimental::StructuringElement>::value);

      auto* val = std::any_cast<image_value_t<I>>(&m_value);
      if (!val)
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_fill::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return {view::value_extended(ima, *val)};
      }
      else
      {
        if (!fit(ima, se) || !ima.extension().is_fill_supported())
        {
          // The SE doesn't fit or the extension doesn't dynamically support fill, we make a view of the image with an
          // adapted dynamic border
          mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                           "does not really support fill (dynamically)."
                           " Consider using a large border.");
          return {view::value_extended(ima, *val)};
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().fill(*val);
          return {ima};
        }
      }
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
        -> managed_image_t<BorderManagementPolicy::Native, Ima, value_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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

      // The SE fits and the extension dynamically support fill, we set the value of the border
      ima.extension().fill(*val);
      return {ima};
    }
  };


  ////////////
  // MIRROR //
  ////////////

  template <typename U>
  class border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Auto, U>
  {
  private:
    std::size_t m_padding;

  public:
    explicit border_manager(std::size_t padding = 0)
      : m_padding{padding}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, mirror_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_mirror::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support mirroring.");
        return {view::mirror_extended(ima, m_padding)};
      }
      else
      {
        if (!fit(ima, se) || !ima.extension().is_mirror_supported())
        {
          // The SE doesn't fit or the extension doesn't dynamically support mirror, we make a view of the image with an
          // adapted dynamic border
          mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                           "does not really support mirror (dynamically)."
                           " Consider using a large border.");
          return {view::mirror_extended(ima, m_padding)};
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().mirror(m_padding);
          return {ima};
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Native, U>
  {
  private:
    std::size_t m_padding;

  public:
    explicit border_manager(std::size_t padding = 0)
      : m_padding{padding}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_image_t<BorderManagementPolicy::Native, Ima, mirror_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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
      ima.extension().mirror(m_padding);
      return {ima};
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
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, periodize_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_periodize::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support periodizing.");
        return {view::periodize_extended(ima)};
      }
      else
      {
        if (!fit(ima, se) || !ima.extension().is_periodize_supported())
        {
          // The SE doesn't fit or the extension doesn't dynamically support periodize, we make a view of the image with
          // an adapted dynamic border
          mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                           "does not really support periodize (dynamically)."
                           " Consider using a large border.");
          return {view::periodize_extended(ima)};
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().periodize();
          return {ima};
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_image_t<BorderManagementPolicy::Native, Ima,
                           periodize_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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
      return {ima};
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
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, clamp_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_clamp::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support clamping.");
        return {view::clamp_extended(ima)};
      }
      else
      {
        if (!fit(ima, se) || !ima.extension().is_clamp_supported())
        {
          // The SE doesn't fit or the extension doesn't dynamically support clamp, we make a view of the image with an
          // adapted dynamic border
          mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                           "does not really support clamp (dynamically)."
                           " Consider using a large border.");
          return {view::clamp_extended(ima)};
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().clamp();
          return {ima};
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
        -> managed_image_t<BorderManagementPolicy::Native, Ima, clamp_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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
      return {ima};
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
        -> managed_image_t<BorderManagementPolicy::Auto, Ima, image_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<I>>);
      static_assert(std::is_convertible_v<image_point_t<I>, image_value_t<U>>);

      if constexpr (!image_has_extension<I>::value || image_extension_t<I>::support_extend_with::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support extend_with filling.");
        return {view::image_extended(ima, m_baseimage)};
      }
      else
      {
        if (!fit(ima, se) || !ima.extension.is_extend_with_supported())
        {
          // The SE doesn't fit or the extension doesn't dynamically support extend_with, we make a view of the image
          // with an adapted dynamic border
          mln::trace::warn("[Performance] Either the extension of the image is two small or the underlying extension "
                           "does not really support extend_with (dynamically)."
                           " Consider using a large border.");
          return {view::image_extended(ima, m_baseimage, m_offset)};
        }
        else
        {
          // The SE fits, we set the value of the border
          // FIXME: to implement
          // ima.extension().extend_with(m_baseimage, m_offset);
          return {ima};
        }
      }
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
        -> managed_image_t<BorderManagementPolicy::Native, Ima, image_extended_view<mln::detail::remove_cvref_t<Ima>>>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);
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
      return {ima};
    }
  };


  //////////
  // USER //
  //////////

  template <typename U>
  class border_manager<BorderManagementMethod::User, BorderManagementPolicy::Auto, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const -> managed_image_t<BorderManagementPolicy::Native, Ima, Ima>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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
      return {ima};
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::User, BorderManagementPolicy::Native, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const -> managed_image_t<BorderManagementPolicy::Native, Ima, Ima>
    {
      using I = mln::detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      // static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

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
      return {ima};
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
