#pragma once

#include <mln/core/concept/new/check.hpp>
#include <mln/core/concept/new/values.hpp>


namespace mln::archetypes
{

  struct Value final
  {
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Value<Value>, "Value archetype does not model the Value concept!");


  struct ComparableValue final
  {
  };
  bool operator==(const ComparableValue&, const ComparableValue&);
  bool operator!=(const ComparableValue&, const ComparableValue&);

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ComparableValue<ComparableValue>,
                           "ComparableValue archetype does not model the ComparableValue concept!");


  struct OrderedValue final
  {
  };
  bool operator==(const OrderedValue&, const OrderedValue&);
  bool operator!=(const OrderedValue&, const OrderedValue&);
  bool operator<(const OrderedValue&, const OrderedValue&);
  bool operator>(const OrderedValue&, const OrderedValue&);
  bool operator<=(const OrderedValue&, const OrderedValue&);
  bool operator>=(const OrderedValue&, const OrderedValue&);

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::OrderedValue<OrderedValue>,
                           "OrderedValue archetype does not model the OrderedValue concept!");

} // namespace mln::archetypes
