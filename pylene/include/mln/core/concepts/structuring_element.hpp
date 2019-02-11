#pragma once

#include <mln/core/concepts/archetype/pixel.hpp>
#include <mln/core/private/traits/neighborhood.hpp>

#include <concepts/concepts.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/functional/concepts.hpp>
#include <type_traits>


namespace mln::details
{
  template <class SE>
  struct StructuringElement
  {
  };
} // namespace mln


namespace mln::concepts
{
  // clang-format off

  namespace details
  {
    template <typename SE>
    concept DynamicStructuringElement =
      requires (SE se) {
      { se.radial_extent() }  -> ::concepts::same_as<int>;
      };


    constexpr bool implies(bool a, bool b) { return !a || b; }
  }


  template <typename SE, typename P>
  concept StructuringElement =
    ::concepts::convertible_to<SE, mln::details::StructuringElement<SE>> &&
    ::ranges::regular_invocable<SE, P> &&
    ::ranges::regular_invocable<SE, mln::archetypes::PixelT<P>> &&
    requires {
      typename SE::category;
      typename SE::incremental;
      typename SE::decomposable;
      typename SE::separable;
    } &&
    ::concepts::convertible_to<typename SE::category, mln::adaptative_neighborhood_tag> &&
    details::implies(::concepts::convertible_to<typename SE::category, mln::dynamic_neighborhood_tag>,
                     details::DynamicStructuringElement<SE>) &&
    requires (SE se, const SE cse, P p, mln::archetypes::PixelT<P> px) {
      { se(p) }         -> ::ranges::cpp20::forward_range;
      { se(px) }        -> ::ranges::cpp20::forward_range;
      { cse.offsets() } -> ::ranges::cpp20::forward_range;

      requires ::concepts::convertible_to<::ranges::range_value_t<decltype(se(p))>, P>;
      requires concepts::Pixel<::ranges::range_value_t<decltype(se(px))>>;
      requires ::concepts::convertible_to<::ranges::range_value_t<decltype(cse.offsets())>, P>;
    };

  namespace details
  {
    template <typename R, typename P>
    concept RangeOfStructuringElement =
      StructuringElement<::ranges::range_value_t<R>, P>;
  }



  template <typename SE, typename P>
  concept DecomposableStructuringElement =
    StructuringElement<SE, P> &&
    ::concepts::convertible_to<typename SE::decomposable, std::true_type> &&
    requires(const SE se) {
      { se.is_decomposable() }  -> ::concepts::same_as<bool>;
      { se.decompose() }        -> ::ranges::cpp20::forward_range;
      requires details::RangeOfStructuringElement<decltype(se.decompose()), P>;
    };


  template <typename SE, typename P>
  concept SeparableStructuringElement =
    StructuringElement<SE, P> &&
    ::concepts::convertible_to<typename SE::separable, std::true_type> &&
    requires(const SE se) {
      { se.is_separable() } -> ::concepts::same_as<bool>;
      { se.separate() }     -> ::ranges::cpp20::forward_range;
      requires details::RangeOfStructuringElement<decltype(se.separate()), P>;
    };


  template <typename SE, typename P>
  concept IncrementalStructuringElement =
    StructuringElement<SE, P> &&
    ::concepts::convertible_to<typename SE::incremental, std::true_type> &&
    requires(const SE se) {
      { se.inc() }  -> StructuringElement<P>;
      { se.dec() }  -> StructuringElement<P>;
    };
  // clang-format on


} // namespace mln::concepts
