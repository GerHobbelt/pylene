#pragma once

#include <algorithm>
#include <functional>
#include <type_traits>
#include <tuple>
#include <range/v3/detail/adl_get.hpp> // for ADL get(x)


/**
 * \file
 * \brief Defines the fundamental logical, arithmetic and relational operators:
 * \see mln::logical_not
 * \see mln::logical_and
 * \see mln::logical_or
 * \see mln::negate
 * \see mln::add
 * \see mln::substract
 * \see mln::multiples
 * \see mln::devides
 * \see mln::modulo
 * \see mln::equals_to
 * \see mln::not_equals_to
 * \see mln::greater_than
 * \see mln::less_than
 * \see mln::greater_equal
 * \see mln::less_equal
 * \see mln::getter
 * \see mln::inf
 * \see mln::sup
 * \see mln::min
 * \see mln::max
 */

namespace mln
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
    std::common_type_t<U, V> operator()(const U& x, const V& y) const { return x + y; }
  };

  template <typename U, typename V = U>
  struct substract
  {
    std::common_type_t<U, V> operator()(const U& x, const V& y) const { return x - y; }
  };

  template <typename U, typename V = U>
  struct multiplies
  {
    std::common_type_t<U, V> operator()(const U& x, const V& y) const { return x * y; }
  };

  template <typename U, typename V = U>
  struct devides
  {
    std::common_type_t<U, V> operator()(const U& x, const V& y) const { return x / y; }
  };

  template <typename U, typename V = U>
  struct modulo
  {
    std::common_type_t<U, V> operator()(const U& x, const V& y) const { return x % y; }
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
    inline auto operator()(E1&& expr1, E2&& expr2, E3&& expr3) const
    {
      return std::forward<E1>(expr1) ? std::forward<E2>(expr2) : std::forward<E3>(expr3);
    }
  };
} // namespace mln

// FIXME: get must be imported in the mln namespace to prevent
// a dependant name lookup that would force the lookup at
// definition instead of the instanciation.
namespace mln
{
  namespace details::_getter
  {
    template <std::size_t N, class C>
    requires(std::is_scalar_v<std::remove_reference_t<C>>)
    decltype(auto) get(C&& obj)
    {
      static_assert(N == 0, "N must be 0 for scalar type");
      return std::forward<C>(obj);
    }

    template <std::size_t N, class C>
    requires (!std::is_scalar_v<std::remove_reference_t<C>>)
    decltype(auto) get(C&& obj)
    {
      // Force ADL look up
      return ::ranges::detail::adl_get<N>(std::forward<C>(obj));
    }
  }

  template <size_t N>
  struct getter
  {
    template <class C>
    decltype(auto) operator()(C&& obj) const
    {
      return details::_getter::get<N>(std::forward<C>(obj));
    }
  };
} // namespace mln

/****************************/
/**  Relational operations **/
/****************************/
namespace mln
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
      using R = std::common_type_t<U, V>;

      R operator()(const U& x, const V& y) const { return std::max<R>(x, y); }
    };

    template <class U, class V = U>
    struct min_t
    {
      using R = std::common_type_t<U, V>;

      R operator()(const U& x, const V& y) const { return std::min<R>(x, y); }
    };

    template <class U, class V = U>
    struct inf_t
    {
      using R = std::common_type_t<U, V>;

      R operator()(const U& x, const V& y) const { return inf<R>(x, y); }
    };

    template <class U, class V = U>
    struct sup_t
    {
      using R = std::common_type_t<U, V>;

      R operator()(const U& x, const V& y) const { return sup<R>(x, y); }
    };
  } // namespace functional

  /*****************************/
  /** Aggregation operations  **/
  /*****************************/

} // namespace mln
