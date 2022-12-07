#pragma once

#include <mln/core/concepts/point.hpp>
#include <mln/core/private/traits/pixel.hpp>

#include <concepts>
#include <type_traits>

namespace mln::details
{
    template <class Px>
    struct Pixel
    {
    };
} // namespace mln::details

namespace mln::concepts
{
  // clang-format off

    template <class Pix> concept Pixel =
      std::is_base_of_v<mln::details::Pixel<Pix>, Pix> &&
      std::copy_constructible<Pix> &&
      std::move_constructible<Pix> &&
      requires {
      typename pixel_value_t<Pix>;
      typename pixel_reference_t<Pix>;
      typename pixel_point_t<Pix>;
    } &&
    std::semiregular<pixel_value_t<Pix>> &&
    Point<pixel_point_t<Pix>> &&
    !std::is_const_v<pixel_value_t<Pix>> &&
    !std::is_reference_v<pixel_value_t<Pix>> &&
    requires(const Pix cpix, Pix pix, pixel_point_t<Pix> p) {
      { cpix.point() } -> std::convertible_to<pixel_point_t<Pix>>;
#if (__GNUG__ == 9) // see https://stackoverflow.com/questions/55198202/unable-to-deduce-placeholder-type-in-concept
      { cpix.val() }   -> std::convertible_to<pixel_reference_t<Pix>>&&;
#else
      { cpix.val() }   -> std::convertible_to<pixel_reference_t<Pix>>;
#endif
      { pix.shift(p) };
    };

  namespace detail
  {

    // WritablePixel
    template <typename WPix>
    concept WritablePixel =
      Pixel<WPix> &&
      requires(const WPix cpix, pixel_value_t<WPix> v) {
        // Not deep-const, view-semantic.
        { cpix.val() = v };
        // Proxy rvalues must not be deep-const on their assignement semantic (unlike tuple...)
        // { const_cast<typename WPix::reference const &&>(cpix.val()) = v };
      };
  } // namespace mln::concepts::detail


  // OutputPixel
  template <typename Pix>
  concept OutputPixel = detail::WritablePixel<Pix>;


  // clang-format on
} // namespace mln::concepts

