#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/value/index.hpp>
#include <mln/core/value/value_traits.hpp>

#include <functional>
#include <type_traits>

namespace mln
{

  template <typename V, typename StrictWeakOrdering, typename Enable = void>
  struct indexer;

  template <typename V, typename StrictWeakOrdering, typename Enable = void>
  struct has_indexer;

  namespace internal
  {
    struct no_indexer_tag
    {
    };
  } // namespace internal

  // template <typename V, typename StrictWeakOrdering>
  // using has_indexer = typename std::integral_constant<bool, not std::is_base_of< internal::no_indexer_tag, indexer<V,
  // StrictWeakOrdering> >::value>;

  template <typename V, typename StrictWeakOrdering, typename Enable>
  struct has_indexer : std::false_type
  {
  };

  // Definition for integral type
  template <typename V>
  struct has_indexer<V, std::less<V>, typename std::enable_if<std::is_integral<V>::value>::type> : std::true_type
  {
  };

  template <typename V>
  struct has_indexer<V, std::greater<V>, typename std::enable_if<std::is_integral<V>::value>::type> : std::true_type
  {
  };

  // Specialization for builtin unsigned integral type
  template <typename V>
  struct indexer<V, std::less<V>,
                 typename std::enable_if<std::is_integral<V>::value and std::is_unsigned<V>::value>::type>
  {
    static_assert(value_traits<V>::quant < value_traits<std::size_t>::quant, "The type is not indexable");

    using index_type = Index<V, std::less<V>>;
    using value_type = V;
    static constexpr bool              inversible = true;
    static constexpr std::size_t       nvalues    = value_traits<V>::max() + 1;

    index_type operator()(value_type x) const { return index_type(x); }
    value_type inv(index_type i) const { return i; }
  };

  template <typename V>
  struct indexer<V, std::greater<V>,
                 typename std::enable_if<std::is_integral<V>::value and std::is_unsigned<V>::value>::type>
  {
    static_assert(value_traits<V>::quant <= value_traits<std::size_t>::quant, "The type is not indexable");

    using index_type = Index<V, std::greater<V>>;
    using value_type = V;
    static constexpr bool                 inversible = true;
    static constexpr std::size_t          nvalues    = value_traits<V>::max() + 1;

    index_type operator()(value_type x) const { return index_type(x); }
    value_type inv(index_type i) const { return i; }
  };

  // Specialization for builtin signed integral type
  // FIXME use the type mln::uint<n>
  template <typename V>
  struct indexer<V, std::less<V>,
                 typename std::enable_if<std::is_integral<V>::value and std::is_signed<V>::value>::type>
  {
    static_assert(value_traits<V>::quant <= value_traits<std::size_t>::quant, "The type is not indexable");

  private:
    using enc = std::size_t;

  public:
    using index_type = Index<enc, std::less<enc>>;
    using value_type = V;
    static const bool                  inversible = true;
    static const std::size_t           nvalues    = value_traits<std::make_unsigned_t<V>>::max() + 1;

    index_type operator()(value_type x) const { return x - value_traits<V>::min(); }
    value_type inv(index_type i) const { return i + value_traits<V>::min(); }
  };

  template <typename V>
  struct indexer<V, std::greater<V>,
                 typename std::enable_if<std::is_integral<V>::value and std::is_signed<V>::value>::type>
  {
  private:
    using enc = std::size_t;

  public:
    using index_type =  Index<enc, std::greater<enc>>;
    using value_type = V;
    static constexpr bool                     inversible = true;
    static constexpr std::size_t              nvalues    = value_traits<std::make_unsigned_t<V>>::max() + 1;

    index_type operator()(value_type x) const { return x - value_traits<V>::min(); }
    value_type inv(index_type i) const { return i + value_traits<V>::min(); }
  };
} // namespace mln
