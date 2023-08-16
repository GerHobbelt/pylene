#pragma once

#include <functional>
#include <limits>
#include <type_traits>


namespace mln
{
  namespace details
  {
    template <typename T>
    inline constexpr bool always_false_v = false;
  }
  /// \brief Object function for a comparison between
  /// two vectorial types using the lexicographical order.
  ///
  /// \f[
  /// a < b \leftrightarrow \exists i s.t. a_i < b_i
  /// \text{ and } \forall j < i \, \neg b_j < a_j
  /// \f]
  template <typename U, typename V = U>
  struct lexicographicalorder_less;

  template <typename U, typename V>
  bool veclex_isless(const U&, const V&);

  template <typename U, typename V>
  bool veclex_islessequal(const U&, const V&);

  template <typename U, typename V>
  bool veclex_isgreater(const U&, const V&);

  template <typename U, typename V>
  bool veclex_isgreaterequal(const U&, const V&);

  /// \brief Object function for a comparison between
  /// two vectorial types using the product order.
  ///
  /// \f[
  /// a \leq b \leftrightarrow \forall i \; a_i \leq b_i
  /// \f]
  /// Strict relation are deduced by reflexive reduction i.e
  ///
  /// \f[
  /// a < b \leftrightarrow a \leq b and a \ne b
  /// \f]
  template <typename U, typename V = U>
  struct productorder_less;

  template <typename U, typename V = U>
  struct productorder_less_equal;

  template <typename U, typename V = U>
  struct productorder_greater;

  template <typename U, typename V = U>
  struct productorder_greater_equal;

  template <typename U, typename V>
  bool vecprod_isless(const U&, const V&) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>);

  template <typename U, typename V>
  bool vecprod_islessequal(const U&, const V&) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>);

  template <typename U, typename V>
  bool vecprod_isgreater(const U&, const V&) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>);

  template <typename U, typename V>
  bool vecprod_isgreaterequal(const U&, const V&) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>);


  template <typename V, typename Ordering = productorder_less<V>>
  struct value_traits
  {
    static_assert(details::always_false_v<V>, "You must specialize this trait for your types.");
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename U, typename V>
  bool vecprod_isless(const U& u, const V& v) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>)
  {
    return u < v;
  }


  template <typename U, typename V>
  bool vecprod_islessequal(const U& u, const V& v) requires(std::is_arithmetic_v<U>and std::is_arithmetic_v<V>)
  {
    return u <= v;
  }


  template <typename U, typename V>
  struct lexicographicalorder_less
  {
    bool operator()(const U& u, const V& v) const { return u < v; }
  };

  template <typename U, typename V>
  struct productorder_less
  {
    bool operator()(const U& u, const V& v) const { return vecprod_isless(u, v); }
  };

  template <typename U, typename V>
  struct productorder_less_equal
  {
    bool operator()(const U& u, const U& v) const { return vecprod_islessequal(u, v); }
  };


  // Default traits for std::greater
  template <typename V>
  struct value_traits<V, std::greater<V>>
  {
    static constexpr unsigned ndim  = value_traits<V, std::less<V>>::ndim;
    static constexpr unsigned quant = value_traits<V, std::less<V>>::quant;
    static constexpr V        min() { return value_traits<V, std::less<V>>::max(); }
    static constexpr V        max() { return value_traits<V, std::less<V>>::min(); }
    static constexpr V        inf() { return value_traits<V, std::less<V>>::sup(); }
    static constexpr V        sup() { return value_traits<V, std::less<V>>::inf(); }
  };

  // Specialization for primitive types
  template <typename V>
  requires std::is_arithmetic_v<V> //
  struct value_traits<V, std::less<V>>
  {
    static constexpr unsigned ndim  = 1;
    static constexpr unsigned quant = sizeof(V) * 8;
    static constexpr V        min() { return std::numeric_limits<V>::lowest(); }
    static constexpr V        max() { return std::numeric_limits<V>::max(); }
    static constexpr V        inf() { return min(); }
    static constexpr V        sup() { return max(); }
  };

  template <typename V>
  requires std::is_arithmetic_v<V> //
  struct value_traits<V, productorder_less<V>> : value_traits<V, std::less<V>>
  {
  };

  template <typename V>
  requires std::is_arithmetic_v<V> //
  struct value_traits<V, lexicographicalorder_less<V>> : value_traits<V, std::less<V>>
  {
  };
} // namespace mln
