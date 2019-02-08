#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <mln/core/image/private/pixel_traits.hpp>

#include <type_traits>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Pixel
  template <typename Pix>
  concept Pixel = 
    // Minimum constraint on image object
    // Do not requires DefaultConstructible
    stl::CopyConstructible<Pix> &&
    stl::MoveConstructible<Pix> &&
    requires {
      typename pixel_value_t<Pix>;
      typename pixel_reference_t<Pix>;
      typename pixel_point_t<Pix>;
    } &&
    Value<pixel_value_t<Pix>> &&
    Point<pixel_point_t<Pix>> &&
    !std::is_const_v<pixel_value_t<Pix>> &&
    !std::is_reference_v<pixel_value_t<Pix>> &&
    requires(const Pix cpix) {
      { cpix.point() } -> stl::ConvertibleTo<pixel_point_t<Pix>>&&;
      { cpix.val() }   -> stl::ConvertibleTo<pixel_reference_t<Pix>>&&;
    };


  namespace detail
  {

    // WritablePixel
    template <typename WPix>
    concept WritablePixel =
      Pixel<WPix> &&
      requires(const WPix cpix, pixel_value_t<WPix> v) {
        // Not deep-const, view-semantic.
        { cpix.val() = v }  -> stl::ConvertibleTo<pixel_reference_t<WPix>>&&;
      };
  
  } // namespace mln::concepts::detail


  // OutputPixel
  template <typename Pix>
  concept OutputPixel = detail::WritablePixel<Pix>;

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/pixel.hpp>
