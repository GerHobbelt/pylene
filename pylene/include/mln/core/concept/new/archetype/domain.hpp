#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/domains.hpp>


namespace mln::archetypes
{

  struct Domain final
  {
    using value_type = Point;
    using reference  = Point&;

    value_type* begin();
    value_type* end();

    bool has(value_type) const;
    bool empty() const;
    // unsigned size() const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Domain<Domain>, "Domain archetype does not model the Domain concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes
