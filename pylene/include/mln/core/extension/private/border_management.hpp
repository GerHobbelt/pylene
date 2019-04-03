#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/new/se.hpp>
#include <mln/core/image/view/extend_with_value.hpp>
#include <mln/core/trace.hpp>

#include <any>
#include <exception>


namespace mln::extension
{

  enum border_management
  {
    BORDER_NONE,
    BORDER_FILL,
    BORDER_PERIODIZE,
    BORDER_MIRROR
  };

  template <border_management method>
  class border_manager;


  /*
  class any_border_manager
  {
  private:
    std::variant<
      border_manager<


    >
  public:

    template <class I, class SE>
    void manage(experimental::Image<I>& f_, experimental::StructuringElement<SE> se_) const
    {
      std::visit(
    }
  };
  */


  template <>
  class border_manager<BORDER_NONE>
  {
  public:
    template <class I, class SE>
    [[gnu::noinline]] void manage(I&& f, const experimental::StructuringElement<SE>& se_) const {
      static_assert(mln::is_a<std::remove_reference_t<I>, experimental::Image>::value);

      const SE& se = static_cast<const SE&>(se_);
      if (mln::extension::need_adjust(f, se))
        throw std::runtime_error("The border management method is said to be NONE.");
    }
  };


  template <>
  class border_manager<BORDER_FILL>
  {
  private:
    std::any m_value;


  public:
    using fail_image_t = void;


    template <class T>
    border_manager(T value)
      : m_value{std::move(value)}
    {
    }

    // template <class I>
    // border_manager(const experimental::Image<I>&, image_value_t<I> value)
    //   : m_value{std::move(value)}
    // {
    // }

    template <class InputImage, class SE>
    [[gnu::noinline]] std::optional<mln::extended_by_value_view<std::decay_t<InputImage>>>
        manage(InputImage&& f, const experimental::StructuringElement<SE>& se_) const {
          using I = std::remove_reference_t<InputImage>;
          static_assert(mln::is_a<I, experimental::Image>::value);

          const SE& se = static_cast<const SE&>(se_);

          auto* val = std::any_cast<image_value_t<I>>(&m_value);
          if (!val)
            throw std::runtime_error(
                "Trying to fill the border with a bad value type. Ensure that value type fits the image type.");

          if constexpr (!image_has_extension<I>::value || !image_extension_t<I>::support_fill::value)
          {
            mln::trace::warn("[Performance] The image has no extension or does not support filling.");
            return view::extend_with_value(f, *val);
          }
          else
          {
            if (mln::extension::need_adjust(f, se))
            {
              mln::trace::warn("[Performance] The extension of the image is two small. Consider using a large border.");
              return view::extend_with_value(f, *val);
            }
            else
            {
              f.extension().fill(*val);
              return std::nullopt;
            }
          }
        }
  };

  namespace bm
  {
    using none = border_manager<BORDER_NONE>;
    using fill = border_manager<BORDER_FILL>;
  } // namespace bm


} // namespace mln::extension
