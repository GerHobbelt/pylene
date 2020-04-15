#pragma once

#include <mln/core/concepts/archetype/pixel.hpp>
#include <mln/core/concepts/archetype/point.hpp>
#include <mln/core/concepts/archetype/structuring_element.hpp>
#include <mln/core/concepts/image.hpp>
#include <mln/core/concepts/value.hpp>

#include <concepts/concepts.hpp>

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

  template <typename Ext, typename Pnt>
  concept Extension =
    std::is_base_of_v<mln::experimental::Extension<Ext>, Ext> &&
    requires {
      typename Ext::support_fill;
      typename Ext::support_mirror;
      typename Ext::support_periodize;
      typename Ext::support_clamp;
      typename Ext::support_extend_with;
    } &&
    Value<typename Ext::value_type> &&
    requires (const Ext cext,
        mln::archetypes::StructuringElement<
          Pnt,
          mln::archetypes::Pixel> se, const Pnt pnt) {
      { cext.fit(se) }      -> bool;
      { cext.extent() }     -> int;
    };

  template <typename Ext>
  concept FillableExtension =
    Extension<Ext> &&
    ::concepts::convertible_to<typename Ext::support_fill, std::true_type> &&
    requires {
      typename Ext::value_type;
    } &&
    requires (Ext ext, const Ext cext, const typename Ext::value_type& v) {
      { ext.fill(v) };
      { cext.is_fill_supported() }  -> bool;
    };

  template <typename Ext>
  concept MirrorableExtension =
    Extension<Ext> &&
    ::concepts::convertible_to<typename Ext::support_mirror, std::true_type> &&
    requires (Ext ext, const Ext cext, std::size_t padding) {
      { ext.mirror() };
      { ext.mirror(padding) };
      { cext.is_mirror_supported() }  -> bool;
    };

  template <typename Ext>
  concept PeriodizableExtension =
    Extension<Ext> &&
    ::concepts::convertible_to<typename Ext::support_periodize, std::true_type> &&
    requires (Ext ext, const Ext cext) {
      { ext.periodize() };
      { cext.is_periodize_supported() }  -> bool;
    };

  template <typename Ext>
  concept ClampableExtension =
    Extension<Ext> &&
    ::concepts::convertible_to<typename Ext::support_clamp, std::true_type> &&
    requires (Ext ext, const Ext cext) {
      { ext.clamp() };
      { cext.is_clamp_supported() }  -> bool;
    };

  template <typename Ext, typename U>
  concept ExtendWithExtension =
    Extension<Ext> &&
    ::concepts::convertible_to<typename Ext::support_extend_with, std::true_type> &&
    InputImage<U> &&
    requires {
      typename Ext::value_type;
      typename Ext::point_type;
    } &&
    ::concepts::convertible_to<typename U::value_type, typename Ext::value_type> &&
    ::concepts::convertible_to<typename Ext::point_type, typename U::point_type> &&
    requires (Ext ext, const Ext cext, U u, typename Ext::point_type offset) {
      { ext.extend_with(u, offset) };
      { cext.is_extend_with_supported() }  -> bool;
    };

#endif
  // clang-format on

} // namespace mln::concepts
