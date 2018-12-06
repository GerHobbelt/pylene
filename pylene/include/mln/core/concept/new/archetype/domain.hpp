#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/domains.hpp>


namespace mln::concepts::archetype
{

  struct domain_archetype final
  {
    using value_type = point_archetype;
    using reference  = point_archetype&;

    value_type* begin();
    value_type* end();

    bool     has(value_type) const;
    bool     empty() const;
    unsigned size() const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Domain<domain_archetype>, "domain_archetype does not model the Domain concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype
