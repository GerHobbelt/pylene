#pragma once

#include <mln/core/concept/new/values.hpp>


namespace mln::concepts::archetype
{

  struct value_archetype final
  {
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Value<value_archetype>, "value_archetype does not model the Value concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct comparable_value_archetype final
  {
  };
  bool operator==(const comparable_value_archetype&, const comparable_value_archetype&);
  bool operator!=(const comparable_value_archetype&, const comparable_value_archetype&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(ComparableValue<comparable_value_archetype>,
                "comparable_value_archetype does not model the ComparableValue concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct ordered_value_archetype final
  {
  };
  bool operator==(const ordered_value_archetype&, const ordered_value_archetype&);
  bool operator!=(const ordered_value_archetype&, const ordered_value_archetype&);
  bool operator<(const ordered_value_archetype&, const ordered_value_archetype&);
  bool operator>(const ordered_value_archetype&, const ordered_value_archetype&);
  bool operator<=(const ordered_value_archetype&, const ordered_value_archetype&);
  bool operator>=(const ordered_value_archetype&, const ordered_value_archetype&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(OrderedValue<ordered_value_archetype>,
                "ordered_value_archetype does not model the OrderedValue concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype
