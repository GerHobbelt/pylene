#pragma once

#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/cmcstl2.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/concept/new/ranges.hpp>
#include <mln/core/neighborhood/neighborhood_traits.hpp>

#include <range/v3/range_traits.hpp>
#include <type_traits>


namespace mln::experimental
{
  template <class SE>
  struct StructuringElement
  {
  };
}


namespace mln::concepts
{
  // clang-format off
#ifdef PYLENE_CONCEPT_TS_ENABLED

  namespace details
  {
    template <typename SE>
    concept DynamicStructuringElement =
      requires (SE se) {
      { se.radial_extent() } -> int
    };


    constexpr bool implies(bool a, bool b) { return !a || b; }
  }


  template <typename SE, typename P>
  concept StructuringElement =
    std::is_base_of_v<mln::experimental::StructuringElement<SE>, SE> &&
    stl::RegularInvocable<SE, P> &&
    stl::RegularInvocable<SE, mln::archetypes::PixelT<P>> &&
    requires {
      typename SE::category;
      typename SE::incremental;
      typename SE::decomposable;
      typename SE::separable;
    } &&
    stl::ConvertibleTo<typename SE::category, mln::adaptative_neighborhood_tag> &&
    details::implies(stl::ConvertibleTo<typename SE::category, mln::dynamic_neighborhood_tag>,
                     details::DynamicStructuringElement<SE>) &&
    requires (SE se, P p, mln::archetypes::PixelT<P> px) {
      { se(p) } -> stl::ForwardRange&&;
      { se(px) } -> stl::ForwardRange&&;

      requires detail::RangeValueTypeConvertibleTo<decltype(se(p)), P>;
      requires detail::RangeValueTypeConvertibleTo<decltype(se(px)), mln::archetypes::PixelT<P>>;
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
    stl::Same<typename SE::decomposable, std::true_type> &&
    requires(const SE se) {
      { se.is_decomposable() } -> bool;
      { se.decompose() } -> stl::ForwardRange&&;
      requires details::RangeOfStructuringElement<decltype(se.decompose()), P>;
    };


  template <typename SE, typename P>
  concept SeparableStructuringElement =
    StructuringElement<SE, P> &&
    stl::Same<typename SE::separable, std::true_type> &&
    requires(const SE se) {
      { se.is_separable() } -> bool;
      { se.separate() } -> stl::ForwardRange&&;
      requires details::RangeOfStructuringElement<decltype(se.separate()), P>;
  };


  template <typename SE, typename P>
  concept IncrementalStructuringElement =
    StructuringElement<SE, P> &&
    stl::Same<typename SE::incremental, std::true_type> &&
    requires(const SE se) {
      { se.inc() } -> StructuringElement<P>&&;
      { se.dec() } -> StructuringElement<P>&&;
    };
#endif
  // clang-format on


} // namespace mln::concepts
