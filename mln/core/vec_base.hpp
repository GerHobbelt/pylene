#ifndef MLN_INTERNAL_VEC_BASE_HH
# define MLN_INTERNAL_VEC_BASE_HH

// FIXME:
// replace boost::common_type by c++11 decltype for type broadcasting
// replace boost::enable_if by c++11 std::enable_if
// add specialization for std::is_arithmetic de <type_traits>

// Visual studio specific warning
# ifdef _MSC_VER
#	pragma warning(disable: 4701)
# endif

#include <type_traits>
#include <boost/tr1/functional.hpp>
#include <boost/utility.hpp>
#include <iostream>
#include <array>



// Element-wise operator
// Ex: vector u,v,w;
// w = u + v; <=> w[i] = u[i] + v[i] forall i
# define VEC_BASE_GEN_EW_OP(Trait, Op)					\
  template <typename U>							\
  typename								\
  std::enable_if< vec_base_traits<tag, U>::Trait, vec_base& >::type	\
  operator Op (const vec_base<U, dim, tag>& o)				\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      v_[i] Op o.v_[i];							\
    return *this;							\
  }

// External operator
// Ex: vector u, v; scalar a;
// u = v + a; <=> u[i] = v[i] + a forall i
# define VEC_BASE_GEN_EXT_OP(Trait, Op)					\
  template <typename U>							\
  typename								\
  std::enable_if< vec_base_traits<tag, U>::Trait, vec_base& >::type	\
  operator Op (const U& o)						\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      v_[i] Op o;							\
    return *this;							\
  }

// Relational operator
// Ex: vector u, v;
// u < v iif u[i] = v[i] forall i
# define VEC_BASE_GEN_REL(Trait, Op)					\
  template <typename U>							\
  typename								\
  std::enable_if< vec_base_traits<tag, U>::Trait, bool >::type	\
  operator Op (const vec_base<U, dim, tag>& o) const			\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      if (!(v_[i] Op o.v_[i]))						\
	return false;							\
    return true;							\
  }

// Outside class, Element-wise operator
# define VEC_BASE_GEN_EW_OP_EXT(TraitName, Op)				\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  std::enable_if< vec_base_traits<tag, U>::TraitName,		\
		      vec_base<typename std::common_type<T, U>::type , dim, tag> \
		      >::type						\
  inline								\
  operator Op (const vec_base<T, dim, tag>& x,				\
	       const vec_base<U, dim, tag>& y)				\
  {									\
    typedef vec_base<typename std::common_type<T, U>::type , dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = x.v_[i] Op y.v_[i];					\
    return r;								\
  }

// Outside class, Relational operator
# define VEC_BASE_GEN_EXT_OP_EXT(TraitName, Op)				\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  boost::lazy_enable_if_c< vec_base_traits<tag,U>::TraitName && std::is_convertible<U,T>::value, \
                           vec_base_helper<std::common_type<T, U>, dim, tag> \
                           >::type                                      \
  inline								\
  operator Op (const vec_base<T, dim, tag>& x,				\
	       const U& y)						\
  {									\
    typedef vec_base<typename std::common_type<T, U>::type , dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = x.v_[i] Op y;						\
    return r;								\
  }									\
									\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  boost::lazy_enable_if_c< vec_base_traits<tag,U>::TraitName && std::is_convertible<U,T>::value, \
                           vec_base_helper<std::common_type<T, U>, dim, tag> \
                           >::type                                      \
  inline								\
  operator Op (const U& y, const vec_base<T, dim, tag>& x)		\
  {									\
    typedef vec_base<typename std::common_type<T, U>::type, dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = x.v_[i] Op y;						\
    return r;								\
  }

namespace mln
{


  namespace internal
  {

    template <typename Tag, typename dummy = void>
    struct vec_base_traits;

    template <typename T, unsigned dim, typename tag = void>
    struct vec_base;

    template <typename T, unsigned dim, typename tag>
    struct vec_base_helper
    {
      typedef vec_base<typename T::type, dim, tag> type;
    };


    template <typename T, unsigned dim, typename tag>
    struct vec_base
    {
    public:
      typedef T value_type;
      typedef T* pointer;
      typedef T& reference;
      typedef T* iterator;
      typedef const T* const_iterator;
      typedef std::reverse_iterator<T*> reverse_iterator;
      typedef std::reverse_iterator<const T*> const_reverse_iterator;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;

      template <typename U>
      vec_base&
      operator= (const vec_base<U, dim, tag>& other)
      {
	std::copy(other.v_, other.v_ + dim, v_);
	return *this;
      }

      T& operator[] (size_type n) { assert(n < dim); return v_[n]; }
      const T& operator[] (size_type n) const { assert(n < dim); return v_[n]; }

      T* begin() { return v_; }
      T* end()   { return v_ + dim; }
      const T* begin() const { return v_; }
      const T* end()   const { return v_ + dim; }

      reverse_iterator rbegin() { return reverse_iterator(v_ + dim); }
      reverse_iterator rend()   { return reverse_iterator(v_); }
      const_reverse_iterator rbegin() const { return const_reverse_iterator(v_ + dim); }
      const_reverse_iterator rend()   const { return const_reverse_iterator(v_); }

      /* Arithmetic operators */
      VEC_BASE_GEN_EW_OP(is_additive, +=)
      VEC_BASE_GEN_EW_OP(is_additive, -=)
      VEC_BASE_GEN_EW_OP(is_multiplicative, *=)
      VEC_BASE_GEN_EW_OP(is_multiplicative, /=)

      /* Ring */
      VEC_BASE_GEN_EXT_OP(is_multiplicative_ext, *=)
      VEC_BASE_GEN_EXT_OP(is_multiplicative_ext, /=)
      VEC_BASE_GEN_EXT_OP(is_additive_ext, -=)
      VEC_BASE_GEN_EXT_OP(is_additive_ext, +=)

      /* RELATIONAL */
      VEC_BASE_GEN_REL(is_equality_comparable, ==)
      VEC_BASE_GEN_REL(is_less_than_comparable, <)
      VEC_BASE_GEN_REL(is_less_than_comparable, >)
      VEC_BASE_GEN_REL(is_less_than_comparable, <=)
      VEC_BASE_GEN_REL(is_less_than_comparable, >=)

      T v_[dim];
    };


    VEC_BASE_GEN_EW_OP_EXT(is_additive, +)
    VEC_BASE_GEN_EW_OP_EXT(is_additive, -)
    VEC_BASE_GEN_EXT_OP_EXT(is_additive_ext, +)
    VEC_BASE_GEN_EXT_OP_EXT(is_additive_ext, -)
    VEC_BASE_GEN_EW_OP_EXT(is_multiplicative, *)
    VEC_BASE_GEN_EW_OP_EXT(is_multiplicative, /)
    VEC_BASE_GEN_EXT_OP_EXT(is_multiplicative_ext, *)
    VEC_BASE_GEN_EXT_OP_EXT(is_multiplicative_ext, /)

    template <typename T, unsigned dim, typename tag>
    inline
    std::ostream&
    operator<< (std::ostream& os, const vec_base<T, dim, tag>& x)
    {
      os << '[';
      for (unsigned i = 0; i < dim-1; ++i)
	os << x.v_[i] << ',';
      os << x.v_[dim-1] << ']';
      return os;
    }

  }

}

# ifdef _MSC_VER
#	pragma warning(default: 4701)
# endif

#endif
