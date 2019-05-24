#pragma once

#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/structuring_element.hpp>
#include <mln/core/concept/new/cmcstl2.hpp>
#include <mln/core/concept/new/images.hpp>
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

  template <typename Ext, typename Pnt>
  concept Extension =
    std::is_base_of_v<mln::experimental::Extension<Ext>, Ext> &&
    requires {
      typename Ext::value_type;
      typename Ext::point_type;
      typename Ext::support_fill;
      typename Ext::support_mirror;
      typename Ext::support_periodize;
      typename Ext::support_clamp;
      typename Ext::support_buffer;
      typename Ext::is_finite;
    } &&
    Value<typename Ext::value_type> &&
    requires (const Ext cext,
        mln::archetypes::StructuringElement<
          Pnt,
          mln::archetypes::Pixel> se, const Pnt pnt) {
      { cext.fit(se) }    -> bool;
      { cext.value(pnt) } -> typename Ext::value_type;
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

  template <typename Ext, typename U>
  concept BufferFillableExtension = 
    Extension<Ext> &&
    std::is_same_v<typename Ext::support_buffer, std::true_type> &&
    InputImage<U> &&
    stl::ConvertibleTo<typename U::value_type, typename Ext::value_type> &&
    stl::ConvertibleTo<typename Ext::point_type, typename U::point_type> &&
    requires (Ext ext, U u) {
      { ext.buffer(u) };
    };

#endif
  // clang-format on

} // namespace mln::concepts