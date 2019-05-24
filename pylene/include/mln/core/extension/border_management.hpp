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

  template <BorderManagementMethod, BorderManagementPolicy = BorderManagementPolicy::Auto, typename U = void>
  class border_manager;


  template <typename U>
  class border_manager<BorderManagementMethod::None, BorderManagementPolicy::Auto, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
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
  class border_manager<BorderManagementMethod::None, BorderManagementPolicy::User, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      static_assert(!image_has_extension<I>::value,
                    "The image has an extension when none is explicitly requested."
                    "Use an image without a border or switch to the auto border management policy!");

      return ima;
    }
  };


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
  class border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::User, U>
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

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_fill::value,
                    "The image's extension does not support the fill method."
                    "Use an image that does or switch to the auto border management policy!");

      if (fit(ima, se))
      {
        // The SE fits, we set the value of the border
        ima.extension().fill(*val);
        return ima;
      }

      throw std::runtime_error(
          "The given image's extension is not large enough to fit the given structuring element."
          "Use an image that has an extension large enough or switch to the auto border management policy!");
    }
  };

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
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_mirror::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support mirroring.");
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
  class border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::User, U>
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

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_mirror::value,
                    "The image's extension does not support the mirror method."
                    "Use an image that does or switch to the auto border management policy!");

      if (fit(ima, se))
      {
        // The SE fits, we set the value of the border
        ima.extension().mirror(m_padding);
        return ima;
      }

      throw std::runtime_error(
          "The given image's extension is not large enough to fit the given structuring element."
          "Use an image that has an extension large enough or switch to the auto border management policy!");
    }
  };


  template <typename U>
  class border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Auto, U>
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
        mln::trace::warn("[Performance] The image has no extension or does not support periodizing.");
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
  class border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::User, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_periodize::value,
                    "The image's extension does not support the periodize method."
                    "Use an image that does or switch to the auto border management policy!");

      if (fit(ima, se))
      {
        // The SE fits, we set the value of the border
        ima.extension().periodize();
        return ima;
      }

      throw std::runtime_error(
          "The given image's extension is not large enough to fit the given structuring element."
          "Use an image that has an extension large enough or switch to the auto border management policy!");
    }
  };

  template <typename U>
  class border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Auto, U>
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
        mln::trace::warn("[Performance] The image has no extension or does not support clamping.");
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
  class border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::User, U>
  {
  public:
    template <class Ima, class SE>
    auto manage(Ima&& ima, const SE& se) const
    {
      using I = detail::remove_cvref_t<Ima>;
      static_assert(mln::is_a<I, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_clamp::value,
                    "The image's extension does not support the clamp method."
                    "Use an image that does or switch to the auto border management policy!");

      if (fit(ima, se))
      {
        // The SE fits, we set the value of the border
        ima.extension().clamp();
        return ima;
      }

      throw std::runtime_error(
          "The given image's extension is not large enough to fit the given structuring element."
          "Use an image that has an extension large enough or switch to the auto border management policy!");
    }
  };


  template <typename U>
  class border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Auto, U>
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
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<I>>);
      static_assert(std::is_convertible_v<image_point_t<I>, image_value_t<U>>);

      if constexpr (!image_has_extension<I>::value || image_extension_t<I>::support_buffer::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support buffer filling.");
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

  template <typename U>
  class border_manager<BorderManagementMethod::Image, BorderManagementPolicy::User, U>
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
      static_assert(std::is_convertible_v<image_value_t<U>, image_value_t<I>>);
      static_assert(std::is_convertible_v<image_point_t<I>, image_value_t<U>>);

      static_assert(image_has_extension<I>::value,
                    "The given image has no extension."
                    "Use an image without an extension or switch to the auto border management policy!");
      static_assert(image_extension_t<I>::support_buffer::value,
                    "The image's extension does not support the buffer filling method."
                    "Use an image that does or switch to the auto border management policy!");

      if (fit(ima, se))
      {
        // The SE fits, we set the value of the border
        // The SE fits, we set the value of the border
        // FIXME: to implement
        // ima.extension().buffer(m_baseimage);
        return ima;
      }

      throw std::runtime_error(
          "The given image's extension is not large enough to fit the given structuring element."
          "Use an image that has an extension large enough or switch to the auto border management policy!");
    }
  };


  namespace bm
  {
    // Default policy
    inline namespace auto_managed
    {
      using none      = border_manager<BorderManagementMethod::None, BorderManagementPolicy::Auto>;
      using fill      = border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::Auto>;
      using mirror    = border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::Auto>;
      using periodize = border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::Auto>;
      using clamp     = border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::Auto>;
      template <typename U>
      using image = border_manager<BorderManagementMethod::Image, BorderManagementPolicy::Auto, U>;
    } // namespace auto_managed

    namespace user_managed
    {
      using none      = border_manager<BorderManagementMethod::None, BorderManagementPolicy::User>;
      using fill      = border_manager<BorderManagementMethod::Fill, BorderManagementPolicy::User>;
      using mirror    = border_manager<BorderManagementMethod::Mirror, BorderManagementPolicy::User>;
      using periodize = border_manager<BorderManagementMethod::Periodize, BorderManagementPolicy::User>;
      using clamp     = border_manager<BorderManagementMethod::Clamp, BorderManagementPolicy::User>;
      template <typename U>
      using image = border_manager<BorderManagementMethod::Image, BorderManagementPolicy::User, U>;
    } // namespace user_managed
  }   // namespace bm


} // namespace mln::extension
