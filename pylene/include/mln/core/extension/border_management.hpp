#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/extension/extension.hpp>
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
  } // namespace detail

  template <typename Ima, typename SE>
  bool fit(const Ima& ima, const SE& se)
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

  template <BorderManagementMethod /*, BorderManagementPolicy*/>
  class border_manager;


  template </*BorderManagementPolicy Policy*/>
  class border_manager<BorderManagementMethod::None /*, Policy*/>
  {
  public:
    template <class Ima, class SE>
    std::optional<mln::none_extended_view<detail::remove_cvref<Ima>>> manage(Ima&& ima, const SE& se) const
    {
      static_assert(mln::is_a<detail::remove_cvref<Ima>, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      if (!fit(ima, se))
      {
        throw std::runtime_error("The border management method is said to be NONE.");
      }
    }
  };


  template </*BorderManagementPolicy Policy*/>
  class border_manager<BorderManagementMethod::Fill /*, Policy*/>
  {
  private:
    std::any m_value;

  public:
    template <class T>
    border_manager(T value)
      : m_value{std::move(value)}
    {
    }

    template <class Ima, class SE>
    std::optional<mln::value_extended_view<detail::remove_cvref<Ima>>> manage(Ima&& ima, const SE& se) const
    {
      static_assert(mln::is_a<detail::remove_cvref<Ima>, mln::experimental::Image>::value);
      static_assert(mln::is_a<SE, mln::experimental::StructuringElement>::value);

      auto* val = std::any_cast<image_value_t<Ima>>(&m_value);
      if (!val)
        throw std::runtime_error(
            "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

      if constexpr (!image_has_extension<Ima>::value || !image_extension_t<Ima>::support_fill::value)
      {
        mln::trace::warn("[Performance] The image has no extension or does not support filling.");
        return view::value_extended(ima, *val);
      }
      else
      {
        if (mln::extension::need_adjust(ima, se))
        {
          mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
          return view::value_extended(ima, *val);
        }
        else
        {
          ima.extension().fill(*val);
          return std::nullopt;
        }
      }
    }
  };

  namespace bm
  {
    using none = border_manager<BorderManagementMethod::None>;
    using fill = border_manager<BorderManagementMethod::Fill>;
  } // namespace bm


} // namespace mln::extension
