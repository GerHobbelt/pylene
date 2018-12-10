#include <mln/core/concept/new/images.hpp>
#include <mln/core/concept/new/archetype/image.hpp>

#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>


namespace concepts = mln::concepts;
namespace archetypes = mln::archetypes;

TEST(Core, Concept_Image)
{
  static_assert(concepts::Image<archetypes::Image>);
}

