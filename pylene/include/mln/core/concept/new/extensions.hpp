#pragma once

#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/structuring_element.hpp>
#include <mln/core/concept/new/cmcstl2.hpp>
#include <mln/core/concept/new/values.hpp>

#include <type_traits>


namespace mln::experimental
{
  template <class E>
  struct Extension
  {
  };
} // namespace mln::experimental


namespace mln::concepts
{

  // clang-format off
#ifdef PYLENE_CONCEPT_TS_ENABLED

  template <typename Ext>
  concept Extension =
    std::is_base_of_v<mln::experimental::Extension<Ext>, Ext> &&
    requires {
      typename Ext::value_type;
      typename Ext::support_fill;
      typename Ext::support_mirror;
      typename Ext::support_periodize;
      typename Ext::support_clamp;
      typename Ext::is_finite;
    } &&
    Value<typename Ext::value_type> &&
    requires (const Ext cext,
        mln::archetypes::StructuringElement<
          mln::archetypes::Point,
          mln::archetypes::Pixel> se) {
      { cext.fit(se) }  -> bool;
    };
  
  template <typename Ext>
  concept BoundedExtension =
    Extension<Ext> &&
    std::is_same_v<typename Ext::is_finite, std::true_type> &&
    requires (const Ext cext) {
      { cext.size() } -> std::size_t;
    };
  
  template <typename Ext>
  concept UnboundedExtension =
    Extension<Ext> &&
    std::is_same_v<typename Ext::is_finite, std::false_type>;

  template <typename Ext>
  concept FillableExtension = 
    Extension<Ext> &&
    std::is_same_v<typename Ext::support_fill, std::true_type> &&
    requires (Ext ext, const typename Ext::value_type& v) {
      { ext.fill(v) };
    };

  template <typename Ext>
  concept MirrorableExtension = 
    Extension<Ext> &&
    std::is_same_v<typename Ext::support_mirror, std::true_type> &&
    requires (Ext ext, std::size_t padding) {
      { ext.mirror() };
      { ext.mirror(padding) };
    };

  template <typename Ext>
  concept PeriodizableExtension = 
    Extension<Ext> &&
    std::is_same_v<typename Ext::support_periodize, std::true_type> &&
    requires (Ext ext) {
      { ext.periodize() };
    };

    template <typename Ext>
  concept ClampableExtension = 
    Extension<Ext> &&
    std::is_same_v<typename Ext::support_clamp, std::true_type> &&
    requires (Ext ext) {
      { ext.clamp() };
    };

#endif
  // clang-format on

} // namespace mln::concepts
