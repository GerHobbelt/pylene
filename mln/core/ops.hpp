#ifndef MLN_CORE_OPS_HPP
# define MLN_CORE_OPS_HPP

# include <functional>

namespace mln
{
  using std::negate;
  using std::logical_and;
  using std::logical_or;
  using std::logical_not;


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



} // end of namespace mln

#endif //!MLN_CORE_OPS_HPP
