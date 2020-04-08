#pragma once

#include <mln/core/concept/values.hpp>


namespace mln::archetypes
{

  struct Value final
  {
  };

  static_assert(mln::concepts::Value<Value>, "Value archetype does not model the Value concept!");


  struct ComparableValue final
  {
  };
  bool operator==(const ComparableValue&, const ComparableValue&);
  bool operator!=(const ComparableValue&, const ComparableValue&);

  static_assert(mln::concepts::ComparableValue<ComparableValue>,
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

  static_assert(mln::concepts::OrderedValue<OrderedValue>,
                           "OrderedValue archetype does not model the OrderedValue concept!");

} // namespace mln::archetypes
