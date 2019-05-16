#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/check.hpp>
#include <mln/core/concept/new/domains.hpp>


namespace mln::archetypes
{

  struct Domain
  {
    using value_type = Point;
    using reference  = Point&;

    value_type* begin();
    value_type* end();

    bool has(value_type) const;
    bool empty() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Domain<Domain>, "Domain archetype does not model the Domain concept!");

  struct SizedDomain final : Domain
  {
    unsigned size() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::SizedDomain<SizedDomain>,
                           "SizedDomain archetype does not model the SizedDomain concept!");

  struct ShapedDomain final : Domain
  {
    value_type shape() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ShapedDomain<ShapedDomain>,
                           "ShapedDomain archetype does not model the ShapedDomain concept!");
} // namespace mln::archetypes
