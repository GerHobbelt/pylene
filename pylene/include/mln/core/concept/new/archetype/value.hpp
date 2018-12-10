#pragma once

#include <mln/core/concept/new/values.hpp>


namespace mln::archetypes
{

  struct Value final
  {
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Value<Value>, "Value archetype does not model the Value concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct ComparableValue final
  {
  };
  bool operator==(const ComparableValue&, const ComparableValue&);
  bool operator!=(const ComparableValue&, const ComparableValue&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ComparableValue<ComparableValue>,
                "ComparableValue archetype does not model the ComparableValue concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OrderedValue final
  {
  };
  bool operator==(const OrderedValue&, const OrderedValue&);
  bool operator!=(const OrderedValue&, const OrderedValue&);
  bool operator<(const OrderedValue&, const OrderedValue&);
  bool operator>(const OrderedValue&, const OrderedValue&);
  bool operator<=(const OrderedValue&, const OrderedValue&);
  bool operator>=(const OrderedValue&, const OrderedValue&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OrderedValue<OrderedValue>,
                "OrderedValue archetype does not model the OrderedValue concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes
