#ifndef MLN_CORE_OPS_HPP
# define MLN_CORE_OPS_HPP

# include <functional>

namespace mln
{
  using std::negate;
  using std::logical_not;

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

  template <typename U, typename V = U>
  struct add : std::binary_function<U, V, typename std::common_type<U,V>::type>
  {
    typename std::common_type<U,V>::type
    operator() (const U& x, const V& y) const
    {
      return x+y;
    }
  };

  template <typename U, typename V = U>
  struct substract : std::binary_function<U, V, typename std::common_type<U,V>::type>
  {
    typename std::common_type<U,V>::type
    operator() (const U& x, const V& y) const
    {
      return x-y;
    }
  };

  template <typename U, typename V = U>
  struct multiplies : std::binary_function<U, V, typename std::common_type<U,V>::type>
  {
    typename std::common_type<U,V>::type
    operator() (const U& x, const V& y) const
    {
      return x*y;
    }
  };

  template <typename U, typename V = U>
  struct devides : std::binary_function<U, V, typename std::common_type<U,V>::type>
  {
    typename std::common_type<U,V>::type
    operator() (const U& x, const V& y) const
    {
      return x/y;
    }
  };

  template <typename U, typename V = U>
  struct modulo : std::binary_function<U, V, typename std::common_type<U,V>::type>
  {
    typename std::common_type<U,V>::type
    operator() (const U& x, const V& y) const
    {
      return x%y;
    }
  };


  template <typename U, typename V = U>
  struct equal_to : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x == y;
    }
  };

  template <typename U, typename V = U>
  struct not_equal_to : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x != y;
    }
  };


  template <typename U, typename V = U>
  struct less_than : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x < y;
    }
  };


  template <typename U, typename V = U>
  struct greater_than : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x > y;
    }
  };

  template <typename U, typename V = U>
  struct less_equal : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x <= y;
    }
  };

  template <typename U, typename V = U>
  struct greater_equal : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x >= y;
    }
  };

  template <typename U, typename V = U>
  struct logical_and : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x && y;
    }
  };

  template <typename U, typename V = U>
  struct logical_or : std::binary_function<U, V, bool>
  {
    bool
    operator() (const U& x, const V& y) const
    {
      return x || y;
    }
  };





} // end of namespace mln

#endif //!MLN_CORE_OPS_HPP
