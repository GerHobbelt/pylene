#pragma once

#include <algorithm>
#include <functional>
#include <range/v3/detail/adl_get.hpp> // for ADL get(x)
#include <tuple>
#include <type_traits>


/**
 * \file
 * \brief Defines the fundamental logical, arithmetic and relational operators:
 * \see ano::logical_not
 * \see ano::logical_and
 * \see ano::logical_or
 * \see ano::negate
 * \see ano::add
 * \see ano::substract
 * \see ano::multiples
 * \see ano::devides
 * \see ano::modulo
 * \see ano::equals_to
 * \see ano::not_equals_to
 * \see ano::greater_than
 * \see ano::less_than
 * \see ano::greater_equal
 * \see ano::less_equal
 * \see ano::getter
 * \see ano::inf
 * \see ano::sup
 * \see ano::min
 * \see ano::max
 */

namespace ano
{
  using std::logical_not;
  using std::negate;

  /**
   * \struct add
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed addition arithmetic operation.
   *
   * \struct substract
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed substraction arithmetic operation.
   *
   * \struct multiplies
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed multiplication arithmetic operation.
   *
   * \struct devides
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed division arithmetic operation.
   *
   * \struct modulo
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed division arithmetic operation.
   *
   * \struct equals_to
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed equality comparison operation.
   *
   * \struct not_equals_to
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed inequality comparison operation.
   *
   * \struct less_than
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed <em>lesser than</em> comparison operation.
   *
   * \struct greater_than
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed <em>greater than</em> comparison operation.
   *
   * \struct less_equal
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed <em>lesser equal</em> comparison operation.
   *
   * \struct greater_equal
   * \tparam U
   * \tparam V
   * \brief A function object for the mixed <em>greater equal</em> comparison operation.
   */

#ifdef _MSC_VER
#pragma warning(disable : 4800)
#endif

  template <typename U, typename V = U>
  struct add
  {
    typename std::common_type<U, V>::type operator()(const U& x, const V& y) const { return x + y; }
  };

  template <typename U, typename V = U>
  struct substract
  {
    typename std::common_type<U, V>::type operator()(const U& x, const V& y) const { return x - y; }
  };

  template <typename U, typename V = U>
  struct multiplies
  {
    typename std::common_type<U, V>::type operator()(const U& x, const V& y) const { return x * y; }
  };

  template <typename U, typename V = U>
  struct devides
  {
    typename std::common_type<U, V>::type operator()(const U& x, const V& y) const { return x / y; }
  };

  template <typename U, typename V = U>
  struct modulo
  {
    typename std::common_type<U, V>::type operator()(const U& x, const V& y) const { return x % y; }
  };

  template <typename U, typename V = U>
  struct equal_to
  {
    bool operator()(const U& x, const V& y) const { return x == y; }
  };

  template <typename U, typename V = U>
  struct not_equal_to
  {
    bool operator()(const U& x, const V& y) const { return x != y; }
  };

  template <typename U, typename V = U>
  struct less_than
  {
    bool operator()(const U& x, const V& y) const { return x < y; }
  };

  template <typename U, typename V = U>
  struct greater_than
  {
    bool operator()(const U& x, const V& y) const { return x > y; }
  };

  template <typename U, typename V = U>
  struct less_equal
  {
    bool operator()(const U& x, const V& y) const { return x <= y; }
  };

  template <typename U, typename V = U>
  struct greater_equal
  {
    bool operator()(const U& x, const V& y) const { return x >= y; }
  };

  template <typename U, typename V = U>
  struct logical_and
  {
    bool operator()(const U& x, const V& y) const { return x && y; }
  };

  template <typename U, typename V = U>
  struct logical_or
  {
    bool operator()(const U& x, const V& y) const { return x || y; }
  };

  struct conditional_ternary
  {

    template <typename E1, typename E2, typename E3>
    inline decltype(auto) operator()(E1&& expr1, E2&& expr2, E3&& expr3) const
    {
      return std::forward<E1>(expr1) ? std::forward<E2>(expr2) : std::forward<E3>(expr3);
    }
  };
} // namespace ano

// FIXME: get must be imported in the ano namespace to prevent
// a dependant name lookup that would force the lookup at
// definition instead of the instanciation.
namespace ano
{
  namespace details::_getter
  {
    template <std::size_t N, class C>
    requires(std::is_scalar_v<std::remove_reference_t<C>>) decltype(auto) get(C&& obj)
    {
      static_assert(N == 0, "N must be 0 for scalar type");
      return std::forward<C>(obj);
    }

    template <std::size_t N, class C>
    requires(!std::is_scalar_v<std::remove_reference_t<C>>) decltype(auto) get(C&& obj)
    {
      // Force ADL look up
      return ::ranges::detail::adl_get<N>(std::forward<C>(obj));
    }
  } // namespace details::_getter

  template <size_t N>
  struct getter
  {
    template <class C>
    decltype(auto) operator()(C&& obj) const
    {
      return details::_getter::get<N>(std::forward<C>(obj));
    }
  };
} // namespace ano

/****************************/
/**  Relational operations **/
/****************************/
namespace ano
{
  using std::max;
  using std::min;

  template <typename U>
  const U& inf(const U& x, const U& y)
  {
    return std::min(x, y);
  }

  template <typename U>
  const U& sup(const U& x, const U& y)
  {
    return std::max(x, y);
  }

  template <typename U, class Compare>
  const U& inf(const U& x, const U& y, Compare cmp)
  {
    return std::min(x, y, cmp);
  }

  template <typename U, class Compare>
  const U& sup(const U& x, const U& y, Compare cmp)
  {
    return std::max(x, y, cmp);
  }

  namespace functional
  {

    template <class U, class V = U>
    struct max_t
    {
      typedef typename std::common_type<U, V>::type R;

      R operator()(const U& x, const V& y) const { return std::max<R>(x, y); }
    };

    template <class U, class V = U>
    struct min_t
    {
      typedef typename std::common_type<U, V>::type R;

      R operator()(const U& x, const V& y) const { return std::min<R>(x, y); }
    };

    template <class U, class V = U>
    struct inf_t
    {
      typedef typename std::common_type<U, V>::type R;

      R operator()(const U& x, const V& y) const { return inf<R>(x, y); }
    };

    template <class U, class V = U>
    struct sup_t
    {
      typedef typename std::common_type<U, V>::type R;

      R operator()(const U& x, const V& y) const { return sup<R>(x, y); }
    };
  } // namespace functional

  /*****************************/
  /** Aggregation operations  **/
  /*****************************/

} // namespace ano
