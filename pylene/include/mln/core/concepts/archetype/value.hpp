#pragma once


namespace mln::archetypes
{

  struct Value
  {
  };

  struct ComparableValue
  {
  };
  bool operator==(const ComparableValue&, const ComparableValue&);
  bool operator!=(const ComparableValue&, const ComparableValue&);


  struct OrderedValue
  {
  };
  bool operator==(const OrderedValue&, const OrderedValue&);
  bool operator!=(const OrderedValue&, const OrderedValue&);
  bool operator<(const OrderedValue&, const OrderedValue&);
  bool operator>(const OrderedValue&, const OrderedValue&);
  bool operator<=(const OrderedValue&, const OrderedValue&);
  bool operator>=(const OrderedValue&, const OrderedValue&);

} // namespace mln::archetypes
