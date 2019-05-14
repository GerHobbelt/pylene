#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>
#include <mln/core/concept/new/values.hpp>
#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/archetype/se.hpp>

#include <type_traits>


namespace mln::experimental
{
  template <class E>
  struct Extension
  {
  };
}


namespace mln::concepts
{

  // clang-format off
#ifdef PYLENE_CONCEPT_TS_ENABLED

  template <typename Ext>
  concept Extension =
    std::is_base_of_v<mln::experimental::Extension<E>, E> &&
    Point<Pnt> &&
    Pixel<Pix> &&
    requires {
      typename Ext::value_type;
      typename Ext::is_finite;
    } &&
    requires (const Ext cext,
        mln::archetypes::StructuringElement<
          mln::archetypes::Point,
          mln::archetypes::Pixel> se) {
      { cext.fit(se) }  -> bool;
    };
  
  template <typename Ext, typename Pnt, typename Pix>
  concept BoundedExtension =
    Extension<Ext> &&
    std::is_same_v<typename Ext::is_finite, std::true_type> &&
    requires (const Ext cext) {
      { cext.size() } -> int;
    };
  
  template <typename Ext, typename Pnt, typename Pix>
  concept UnboundedExtension =
    Extension<Ext> &&
    std::is_same_v<typename Ext::is_finite, std::false_type>;

#endif
  // clang-format on

} // namespace mln::concepts
