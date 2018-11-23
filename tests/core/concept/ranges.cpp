#include <mln/core/concept/new/ranges.hpp>
#include <mln/core/rangev3/multi_indices.hpp>
// #include <mln/core/rangev3/view/transform.hpp>

#include <range/v3/view/iota.hpp>

#include <array>
#include <vector>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


TEST(Core, Concept_SegmentedRange)
{
  // FIXME

  [[maybe_unused]] mln::ranges::multi_indices<2>   ids({3, 4});
  [[maybe_unused]] std::vector<std::array<int, 2>> ref = {{4, 9}, {4, 4}, {4, 1}, {4, 0}, //
                                                          {1, 9}, {1, 4}, {1, 1}, {1, 0}, //
                                                          {0, 9}, {0, 4}, {0, 1}, {0, 0}};
  /*
  auto pow2_seg = mln::ranges::view::transform(ids, [](auto vals) {
    return std::array<int, 2>{vals[0] * vals[0], vals[1] * vals[1]};
  });
  */

  // static_assert(concepts::SegmentedRange<decltype(pow2_seg)>);

  /*
  std::vector<int> v    = {1, 2, 3, 4, 5};
  auto             pow2 = mln::ranges::view::transform(v, [](auto val) { return val * val; });
  static_assert(!concepts::SegmentedRange<decltype(pow2)>);
  */

  auto iota = ::ranges::view::iota(0, 5);
  static_assert(!concepts::SegmentedRange<decltype(iota)>);
}


TEST(Core, Concept_ReversibleRange)
{
  // FIXME

  [[maybe_unused]] mln::ranges::multi_indices<2>   ids({3, 4});
  [[maybe_unused]] std::vector<std::array<int, 2>> ref = {{4, 9}, {4, 4}, {4, 1}, {4, 0}, //
                                                          {1, 9}, {1, 4}, {1, 1}, {1, 0}, //
                                                          {0, 9}, {0, 4}, {0, 1}, {0, 0}};
  /*
  auto pow2_rev = mln::ranges::view::transform(ids, [](auto vals) {
    return std::array<int, 2>{vals[0] * vals[0], vals[1] * vals[1]};
  });
  */

  // static_assert(concepts::ReversibleRange<decltype(pow2_rev)>);

  /*
  std::vector<int> v    = {1, 2, 3, 4, 5};
  auto             pow2 = mln::ranges::view::transform(v, [](auto val) { return val * val; });
  static_assert(!concepts::ReversibleRange<decltype(pow2)>);
  */
  auto iota = ::ranges::view::iota(0, 5);
  static_assert(!concepts::ReversibleRange<decltype(iota)>);
}
