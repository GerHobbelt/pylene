#pragma once

#include <concepts/concepts.hpp>

#include <mln/core/concept/points.hpp>
#include <mln/core/concept/values.hpp>
#include <mln/core/concept/indexes.hpp>
#include <mln/core/private/traits/pixel.hpp>

#include <type_traits>

namespace mln
{
  namespace experimental
  {
    template <class Px>
    struct Pixel
    {
    };
  } // namespace experimental

  namespace concepts
  {
    // clang-format off


    template <class Pix> concept Pixel =
      std::is_base_of_v<mln::experimental::Pixel<Pix>, Pix> &&
      ::concepts::copy_constructible<Pix> &&
      ::concepts::move_constructible<Pix> &&
      requires {
      typename pixel_value_t<Pix>;
      typename pixel_reference_t<Pix>;
      typename pixel_point_t<Pix>;
    } &&
    Value<pixel_value_t<Pix>> &&
    Point<pixel_point_t<Pix>> &&
    !std::is_const_v<pixel_value_t<Pix>> &&
    !std::is_reference_v<pixel_value_t<Pix>> &&
    requires(const Pix cpix, Pix pix, pixel_point_t<Pix> p) {
      { cpix.point() } -> ::concepts::convertible_to<pixel_point_t<Pix>>;
#if (__GNUG__) // see https://stackoverflow.com/questions/55198202/unable-to-deduce-placeholder-type-in-concept
      { cpix.val() }   -> ::concepts::convertible_to<pixel_reference_t<Pix>>&&;
#else
      { cpix.val() }   -> ::concepts::convertible_to<pixel_reference_t<Pix>>;
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

  } // namespace concepts

} // namespace mln
