#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/image_extended.hpp>
#include <mln/core/image/view/none_extended.hpp>
#include <mln/core/image/view/pattern_extended.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/utility/common_type.hpp> // common_reference

#include <variant>


namespace mln
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


    struct get_image_extension_category_fn
    {
      template <typename Ima>
      image_extension_category_t<Ima> operator()(Ima&&) const
      {
        return {};
      }
    };

    struct get_image_extension_fn
    {
      template <typename Ima>
      image_extension_t<Ima> operator()(Ima&& ima) const
      {
        return std::forward<Ima>(ima).extension();
      }
    };

    struct get_image_new_pixel_fn
    {
      template <typename Ima>
      image_pixel_t<Ima> operator()(Ima&& ima) const
      {
        return std::forward<Ima>(ima).new_pixel(std::declval<image_point_t<Ima>>());
      }
    };
  } // namespace detail

  template <class I, class... Args>
  class extended_view;

  namespace view
  {
    template <class I, class... Args>
    extended_view<I> extended(I image, Args&&... args);
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class I, class... Args>
  class extended_view : public image_adaptor<I>, public experimental::Image<extended_view<I, Args...>>
  {
    using base_t          = image_adaptor<I>;
    using adapted_image_t = std::variant<I, // can be the base image (se fit, no adaptation required)
                                         image_extended_view<I, Args...>, none_extended_view<I>,
                                         pattern_extended_view<I, Args{}...>, value_extended_view<I>>;

  public:
    using extension_category = detail::remove_cvref_t<decltype(
        std::visit(std::declval<adapted_image_t>(), std::declval<detail::get_image_extension_category_fn>()))>;
    using extension_type     = detail::remove_cvref_t<decltype(
        std::visit(std::declval<adapted_image_t>(), std::declval<detail::get_image_extension_fn>()))>;
    using reference          = const image_value_t<I>&; // Restrict the image to be read-only
    using category_type      = std::common_type_t<image_category_t<I>, bidirectional_image_tag>;
    using point_type         = image_point_t<I>;
    using typename image_adaptor<I>::domain_type;
    using new_pixel_type = detail::remove_cvref_t<decltype(
        std::visit(std::declval<adapted_image_t>(), std::declval<detail::get_image_new_pixel_fn>()))>;

  private:
    adapted_image_t m_adapted_image;

  public:
    extended_view(I ima, Args&&... args)
      : base_t{std::move(ima)}
      , m_adapted_image{std::move(ima), args...}
    {
    }

    /// Accessible-image related methods
    /// \{
    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> operator()(point_type p)
    {
      return this->at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, reference> at(point_type p)
    {
      return std::visit(m_adapted_image, [p](auto ima) { return ima.at(p); });
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel(point_type p)
    {
      return this->new_pixel_at(p);
    }

    template <class J = I>
    std::enable_if_t<image_accessible_v<J>, new_pixel_type> new_pixel_at(point_type p)
    {
      return std::visit(m_adapted_image, [p](auto ima) { return ima.new_pixel_at(p); });
    }
    /// \}

    auto new_pixels()
    {
      return std::visit(m_adapted_image, [](auto ima) { return ima.new_pixels(); });
    }

    const extension_type& extension()
    {
      return std::visit(m_adapted_image, [](auto ima) { return ima.extension(); });
    }
  };


  namespace view
  {
    template <class I, class... Args>
    extended_view<I> extended(I image, Args&&... args)
    {
      return {std::move(image), std::forward<Args>(args)...};
    }
  }; // namespace view

} // namespace mln
