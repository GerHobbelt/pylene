#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/pattern_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/trace.hpp>

#include <any>
#include <exception>


namespace mln::extension
{
  namespace detail
  {
    template <class T>
    struct remove_cvref
    {
      using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };
    template <class T>
    using remove_cvref_t = typename remove_cvref<T>::type;
  } // namespace detail

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
    Image
  };

  enum class BorderManagementPolicy
  {
    User,
    Auto
  };

  template <BorderManagementMethod, typename U = void>
  class border_manager;


  template <typename U>
  class border_manager<BorderManagementMethod::None, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      static_assert(mln::is_a<detail::remove_cvref_t<Ima>, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if (!fit(ima, se))
      {
        // The SE doesn't fit, no need to remove the border
        return mln::view::extended(ima);
      }
      else
      {
        // The SE fits, we remove the border
        return mln::view::extended(ima, view::none_extended(ima));
      }
    }
  };


  template <typename U>
  class border_manager<BorderManagementMethod::Fill, U>
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
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      auto* val = std::any_cast<image_value_t<I>>(&m_value);
      if (!val)
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");
      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_fill::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::extended(ima, view::value_extended(ima, *val));
      }
      else
      {
        if (!fit(ima, se))
        {
          // The SE doesn't fit, we make a view of the image with an adapted dynamic border
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::extended(ima, view::value_extended(ima, *val));
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().fill(*val);
          return view::extended(ima);
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Mirror, U>
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
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_mirror::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Mirror, m_padding));
      }
      else
      {
        if (!fit(ima, se))
        {
          // The SE doesn't fit, we make a view of the image with an adapted dynamic border
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Mirror, m_padding));
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().mirror(m_padding);
          return view::extended(ima);
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Periodize, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_periodize::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Periodize));
      }
      else
      {
        if (!fit(ima, se))
        {
          // The SE doesn't fit, we make a view of the image with an adapted dynamic border
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Periodize));
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().periodize();
          return view::extended(ima);
        }
      }
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Clamp, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_clamp::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Clamp));
      }
      else
      {
        if (!fit(ima, se))
        {
          // The SE doesn't fit, we make a view of the image with an adapted dynamic border
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::extended(ima, view::pattern_extended(ima, experimental::Pattern::Clamp));
        }
        else
        {
          // The SE fits, we set the value of the border
          ima.extension().clamp();
          return view::extended(ima);
        }
      }
    }
  };


  template <typename U>
  class border_manager<BorderManagementMethod::Image, U>
  {
  private:
    U m_baseimage;

  public:
    explicit border_manager(U baseimage)
      : m_baseimage{std::move(baseimage)}
    {
    }

    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<Ima>>);
      static_assert(std::is_convertible_v<image_point_t<Ima>, image_value_t<U>>);

      if constexpr (!image_has_extension<I>::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::extended(ima, view::image_extended(ima, m_baseimage));
      }
      else
      {
        if (!fit(ima, se))
        {
          // The SE doesn't fit, we make a view of the image with an adapted dynamic border
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::extended(ima, view::image_extended(ima, m_baseimage));
        }
        else
        {
          // The SE fits, we set the value of the border
          // FIXME: to implement
          // ima.extension().buffer(m_baseimage);
          return view::extended(ima);
        }
      }
    }
  };


  namespace bm
  {
    using none      = border_manager<BorderManagementMethod::None>;
    using fill      = border_manager<BorderManagementMethod::Fill>;
    using mirror    = border_manager<BorderManagementMethod::Mirror>;
    using periodize = border_manager<BorderManagementMethod::Periodize>;
    using clamp     = border_manager<BorderManagementMethod::Clamp>;
    template <typename U>
    using image = border_manager<BorderManagementMethod::Image, U>;
  } // namespace bm


} // namespace mln::extension
