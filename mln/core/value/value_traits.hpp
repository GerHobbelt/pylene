#ifndef VALUE_TRAITS_HPP
# define VALUE_TRAITS_HPP

# include <functional>
# include <limits>

namespace mln
{

  template <typename V, typename StrictWeakOrdering = std::less<V>, class Enable = void>
  struct value_traits;

  template <typename V, class Enable = void>
  struct value_traits_base;

  // Specilization for primitive types
  template <typename V>
  struct value_traits_base<V, typename std::enable_if<std::is_arithmetic<V>::value>::type>
  {
    static constexpr unsigned quant = sizeof(V) * 8;
  };

  template <typename V, typename StrictWeakOrdering>
  struct value_traits<V, StrictWeakOrdering, typename std::enable_if<std::is_arithmetic<V>::value>::type>
    : value_traits_base<V>
  {
  };

  template <typename V>
  struct value_traits<V, std::less<V>, typename std::enable_if<std::is_arithmetic<V>::value>::type>
    : value_traits_base<V>
  {
    static constexpr V min() { return std::numeric_limits<V>::lowest(); }
    static constexpr V max() { return std::numeric_limits<V>::max(); }
    static constexpr V inf() { return min(); }
    static constexpr V sup() { return max(); }
  };

  template <typename V>
  struct value_traits<V, std::greater<V>, typename std::enable_if<std::is_arithmetic<V>::value>::type>
    : value_traits_base<V>
  {
    static constexpr V min() { return std::numeric_limits<V>::max(); }
    static constexpr V max() { return std::numeric_limits<V>::lowest(); }
    static constexpr V inf() { return min(); }
    static constexpr V sup() { return max(); }
  };

}

#endif // ! VALUE_TRAITS_HPP
