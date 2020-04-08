#include <mln/core/concept/archetype/structuring_element.hpp>
#include <mln/core/concept/structuring_elements.hpp>

#include <gtest/gtest.h>

TEST(Core, Concept_SE)
{
  static_assert(mln::concepts::StructuringElement<mln::archetypes::StructuringElement<>, mln::archetypes::Point>);
  static_assert(mln::concepts::DecomposableStructuringElement<mln::archetypes::DecomposableStructuringElement<>,
                                                              mln::archetypes::Point>);
  static_assert(mln::concepts::SeparableStructuringElement<mln::archetypes::SeparableStructuringElement<>,
                                                           mln::archetypes::Point>);
  static_assert(mln::concepts::IncrementalStructuringElement<mln::archetypes::IncrementalStructuringElement<>,
                                                             mln::archetypes::Point>);
}
