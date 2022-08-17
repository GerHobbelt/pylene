#pragma once

#include <ano/core/concepts/archetype/pixel.hpp>
#include <ano/core/concepts/archetype/point.hpp>
#include <ano/core/concepts/archetype/structuring_element.hpp>
#include <ano/core/concepts/image.hpp>
#include <ano/core/concepts/value.hpp>

#include <concepts/concepts.hpp>

#include <type_traits>


namespace ano
{
  template <class E>
  struct Extension
  {
  };
} // namespace ano


namespace ano::concepts
{

  // clang-format off

  template <typename Ext, typename Pnt>
  concept Extension =
    std::is_base_of_v<ano::Extension<Ext>, Ext> &&
    requires {
      typename Ext::support_fill;
      typename Ext::support_mirror;
      typename Ext::support_periodize;
      typename Ext::support_clamp;
      typename Ext::support_extend_with;
    } &&
    Value<typename Ext::value_type> &&
    requires (const Ext cext,
        ano::archetypes::StructuringElement<
          Pnt,
          ano::archetypes::Pixel> se) {
      { cext.fit(se) }      -> ::concepts::same_as<bool>;
      { cext.extent() }     -> ::concepts::same_as<int>;
    };

  template <typename Ext, typename Pnt>
  concept FillableExtension =
    Extension<Ext, Pnt> &&
    ::concepts::convertible_to<typename Ext::support_fill, std::true_type> &&
    requires {
      typename Ext::value_type;
    } &&
    requires (Ext ext, const Ext cext, const typename Ext::value_type& v) {
      { ext.fill(v) };
      { cext.is_fill_supported() }  -> ::concepts::same_as<bool>;
    };

  template <typename Ext, typename Pnt>
  concept MirrorableExtension =
    Extension<Ext, Pnt> &&
    ::concepts::convertible_to<typename Ext::support_mirror, std::true_type> &&
    requires (Ext ext, const Ext cext) {
      { ext.mirror() };
      { cext.is_mirror_supported() }  -> ::concepts::same_as<bool>;
    };

  template <typename Ext, typename Pnt>
  concept PeriodizableExtension =
    Extension<Ext, Pnt> &&
    ::concepts::convertible_to<typename Ext::support_periodize, std::true_type> &&
    requires (Ext ext, const Ext cext) {
      { ext.periodize() };
      { cext.is_periodize_supported() }  -> ::concepts::same_as<bool>;
    };

  template <typename Ext, typename Pnt>
  concept ClampableExtension =
    Extension<Ext, Pnt> &&
    ::concepts::convertible_to<typename Ext::support_clamp, std::true_type> &&
    requires (Ext ext, const Ext cext) {
      { ext.clamp() };
      { cext.is_clamp_supported() }  -> ::concepts::same_as<bool>;
    };

  template <typename Ext, typename Pnt, typename U>
  concept ExtendWithExtension =
    Extension<Ext, Pnt> &&
    ::concepts::convertible_to<typename Ext::support_extend_with, std::true_type> &&
    // InputImage<U> &&
    requires {
      typename Ext::point_type;
    } &&
    ::concepts::convertible_to<typename U::value_type, typename Ext::value_type> &&
    ::concepts::convertible_to<typename Ext::point_type, typename U::point_type> &&
    requires (Ext ext, const Ext cext, U u, typename Ext::point_type offset) {
      { ext.extend_with(u, offset) };
      { cext.is_extend_with_supported() }  -> ::concepts::same_as<bool>;
    };

  // clang-format on

} // namespace ano::concepts
