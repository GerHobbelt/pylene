#ifndef MLN_INTERNAL_VEC_BASE_HH
# define MLN_INTERNAL_VEC_BASE_HH

# include <mln/core/literal/vectorial.hpp>
# include <type_traits>


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
#include <mln/core/assert.hpp>


# define VEC_BASE_ENABLE_IF(Trait, R)					\
  typename std::enable_if< vec_base_traits<tag>::Trait, R>::type

# define VEC_BASE_ENABLE_IFC(Trait, R)					\
  typename std::enable_if< vec_base_traits<tag>::Trait and std::is_convertible<T,U>::value, R>::type


// Element-wise operator
// Ex: vector u,v,w;
// w = u + v; <=> w[i] = u[i] + v[i] forall i
# define VEC_BASE_GEN_EW_OP(Trait, Op)					\
  template <typename U, typename = typename				\
	    std::enable_if<vec_base_traits<tag>::Trait, U>::type>	\
  vec_base&								\
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
  template <typename U, typename = typename				\
	    std::enable_if<vec_base_traits<tag>::Trait, U>::type>	\
  vec_base&								\
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
  template <typename U, typename = typename				\
	    std::enable_if<vec_base_traits<tag>::Trait, U>::type>	\
  bool									\
  operator Op (const vec_base<U, dim, tag>& o) const			\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      if (!(v_[i] Op o.v_[i]))						\
	return false;							\
    return true;							\
  }

// Relational operator
// Ex: vector u, v;
// u op v iif u[i] op v[i] forall i
# define VEC_BASE_GEN_REL_ALL(Trait, Op)				\
  template <typename U>							\
  typename								\
  std::enable_if< vec_base_traits<tag>::Trait, bool >::type		\
  operator Op (const vec_base<U, dim, tag>& o) const			\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      if (!(v_[i] Op o.v_[i]))						\
	return false;							\
    return true;							\
  }


// Relational operator
// Ex: vector u, v;
// u op v iif exists i u[i] op v[i]
# define VEC_BASE_GEN_REL_ANY(Trait, Op)				\
  template <typename U>							\
  typename								\
  std::enable_if< vec_base_traits<tag>::Trait, bool >::type		\
  operator Op (const vec_base<U, dim, tag>& o) const			\
  {									\
    for (unsigned i = 0; i < dim; ++i)					\
      if (v_[i] Op o.v_[i])						\
	return true;							\
    return false;							\
  }



// Outside class, Element-wise operator
# define VEC_BASE_GEN_EW_OP_EXT(TraitName, Op)				\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  std::enable_if< vec_base_traits<tag>::TraitName,		\
		      vec_base<decltype( std::declval<T>() + std::declval<U>() ) , dim, tag> \
		      >::type						\
  inline								\
  operator Op (const vec_base<T, dim, tag>& x,				\
	       const vec_base<U, dim, tag>& y)				\
  {									\
    typedef vec_base<decltype( std::declval<T>() + std::declval<U>() ) , dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = x.v_[i] Op y.v_[i];					\
    return r;								\
  }

// Outside class, Relational operator
# define VEC_BASE_GEN_EXT_OP_EXT(TraitName, Op)				\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  boost::lazy_enable_if_c< vec_base_traits<tag>::TraitName && std::is_convertible<U,T>::value, \
                           vec_base_helper<std::common_type<T, U>, dim, tag> \
                           >::type                                      \
  inline								\
  operator Op (const vec_base<T, dim, tag>& x,				\
	       const U& y)						\
  {									\
    typedef vec_base<decltype( std::declval<T>() + std::declval<U>() ) , dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = x.v_[i] Op y;						\
    return r;								\
  }									\
									\
  template <typename T, typename U, unsigned dim, typename tag>		\
  typename								\
  boost::lazy_enable_if_c< vec_base_traits<tag>::TraitName && std::is_convertible<U,T>::value, \
                           vec_base_helper<std::common_type<T, U>, dim, tag> \
                           >::type                                      \
  inline								\
  operator Op (const U& y, const vec_base<T, dim, tag>& x)		\
  {									\
    typedef vec_base<decltype( std::declval<T>() + std::declval<U>() ), dim, tag> R; \
    R r;								\
    for (unsigned i = 0; i < dim; ++i)					\
      r.v_[i] = y Op x.v_[i];						\
    return r;								\
  }




namespace mln
{

  struct generic_vector_tag {};


  namespace internal
  {

    template <typename Tag>
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

      enum { ndim = dim };

      vec_base() = default;

      constexpr vec_base(const literal::zero_t&)
        : v_ {0,}
      {
      }

      vec_base(const literal::one_t&)
      {
        this->set_all(1);
      }


      template <typename dummy = void>
      constexpr
      vec_base(const T& x, typename std::enable_if<dim==1, dummy>::type* = NULL)
        : v_ {x}
      {
      }

      template <typename dummy = void>
      constexpr
      vec_base(const T& x, const T& y, typename std::enable_if<dim==2, dummy>::type* = NULL)
        : v_ {x,y}
      {
      }


      template <typename dummy = void>
      constexpr
      vec_base(const T& x, const T& y, const T& z, typename std::enable_if<dim==3, dummy>::type* = NULL)
        : v_ {x,y,z}
      {
      }


      template <typename U>
      vec_base(const vec_base<U, dim, tag>& other)
      {
	std::copy(other.v_, other.v_ + dim, v_);
      }


      template <typename U, typename other_tag>
      explicit
      vec_base(const vec_base<U, dim, other_tag>& other)
      {
	std::copy(other.v_, other.v_ + dim, v_);
      }


      void set_all(const T& v)
      {
        std::fill(v_, v_ + dim, v);
      }

      template <typename U>
      vec_base&
      operator= (const vec_base<U, dim, tag>& other)
      {
	std::copy(other.v_, other.v_ + dim, v_);
	return *this;
      }

      T& operator[] (size_type n) { mln_precondition(n < dim); return v_[n]; }
      const T& operator[] (size_type n) const { mln_precondition(n < dim); return v_[n]; }

      vec_base<T, dim, generic_vector_tag>&
      as_vec()
      {
	return *reinterpret_cast< vec_base<T, dim, generic_vector_tag>* >(this);
      }

      const vec_base<T, dim, generic_vector_tag>&
      as_vec() const
      {
	return *reinterpret_cast<const vec_base<T, dim, generic_vector_tag>* >(this);
      }

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
      VEC_BASE_GEN_EW_OP(is_multiplicative, %=)

      /* Ring */
      VEC_BASE_GEN_EXT_OP(is_multiplicative_ext, *=)
      VEC_BASE_GEN_EXT_OP(is_multiplicative_ext, /=)
      VEC_BASE_GEN_EXT_OP(is_multiplicative_ext, %=)
      VEC_BASE_GEN_EXT_OP(is_additive_ext, -=)
      VEC_BASE_GEN_EXT_OP(is_additive_ext, +=)


      vec_base operator - () const
      {
        vec_base out;
        for (unsigned i = 0; i < dim; ++i)
          out.v_[i] = -v_[i];
        return out;
      }



      /* RELATIONAL */
      template <typename U>
      VEC_BASE_ENABLE_IFC(is_equality_comparable, bool)
      operator== (const vec_base<U, dim, tag>& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (!(v_[i] == o.v_[i]))
	    return false;
	return true;
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_equality_comparable, bool)
      operator== (const vec_base& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (!(v_[i] == o.v_[i]))
	    return false;
	return true;
      }


      template <typename U>
      VEC_BASE_ENABLE_IFC(is_equality_comparable, bool)
      operator!= (const vec_base<U, dim, tag>& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] != o.v_[i])
	    return true;
	return false;
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_equality_comparable, bool)
      operator!= (const vec_base& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] != o.v_[i])
	    return true;
	return false;
      }


      template <typename U>
      VEC_BASE_ENABLE_IFC(is_less_than_comparable, bool)
      operator< (const vec_base<U, dim, tag>& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] < o.v_[i])
	    return true;
	  else if (v_[i] > o.v_[i])
	    return false;
	return false;
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_less_than_comparable, bool)
      operator< (const vec_base& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] < o.v_[i])
	    return true;
	  else if (v_[i] > o.v_[i])
	    return false;
	return false;
      }



      template <typename U>
      VEC_BASE_ENABLE_IFC(is_less_than_comparable, bool)
      operator<= (const vec_base<U, dim, tag>& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] < o.v_[i])
	    return true;
	  else if (v_[i] > o.v_[i])
	    return false;
	return true;
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_less_than_comparable, bool)
      operator<= (const vec_base& o) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  if (v_[i] < o.v_[i])
	    return true;
	  else if (v_[i] > o.v_[i])
	    return false;
	return true;
      }


      template <typename U>
      VEC_BASE_ENABLE_IFC(is_less_than_comparable, bool)
      operator > (const vec_base<U, dim, tag>& o) const
      {
	return not(*this <= o);
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_less_than_comparable, bool)
      operator > (const vec_base& o) const
      {
	return not(*this <= o);
      }


      template <typename U>
      VEC_BASE_ENABLE_IFC(is_less_than_comparable, bool)
      operator >= (const vec_base<U, dim, tag>& o) const
      {
	return not(*this < o);
      }

      template <typename dummy=void>
      VEC_BASE_ENABLE_IF(is_less_than_comparable, bool)
      operator >= (const vec_base& o) const
      {
	return not(*this < o);
      }


      template <typename OtherTag>
      operator vec_base<T, dim, OtherTag>& ()
      {
	static_assert( std::is_same<tag, generic_vector_tag>::value,
		       "Only a generic a vector can be implicitely converted"
		       "to another vector type. Use as_vec() method.");
	return * reinterpret_cast< vec_base<T, dim, OtherTag>* > (this);
      }

      template <typename OtherTag>
      operator const vec_base<T, dim, OtherTag>& () const
      {
	static_assert( std::is_same<tag, generic_vector_tag>::value,
		       "Only a generic a vector can be implicitely converted"
		       "to another vector type. Use as_vec() method.");
	return * reinterpret_cast< const vec_base<T, dim, OtherTag>* > (this);
      }

      template <typename SumType = T>
      SumType sum(SumType s = 0) const
      {
	for (unsigned i = 0; i < dim; ++i)
	  s += v_[i];
	return s;
      }

      T v_[dim];
    };


    VEC_BASE_GEN_EW_OP_EXT(is_additive, +)
    VEC_BASE_GEN_EW_OP_EXT(is_additive, -)
    VEC_BASE_GEN_EXT_OP_EXT(is_additive_ext, +)
    VEC_BASE_GEN_EXT_OP_EXT(is_additive_ext, -)
    VEC_BASE_GEN_EW_OP_EXT(is_multiplicative, *)
    VEC_BASE_GEN_EW_OP_EXT(is_multiplicative, /)
    VEC_BASE_GEN_EW_OP_EXT(is_multiplicative, %)
    VEC_BASE_GEN_EXT_OP_EXT(is_multiplicative_ext, *)
    VEC_BASE_GEN_EXT_OP_EXT(is_multiplicative_ext, /)
    VEC_BASE_GEN_EXT_OP_EXT(is_multiplicative_ext, %)

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

    template <>
    struct vec_base_traits<generic_vector_tag>
    {
      static const bool is_additive = true;
      static const bool is_additive_ext = true;
      static const bool is_multiplicative = true;
      static const bool is_multiplicative_ext = true;
      static const bool is_less_than_comparable = true;
      static const bool is_equality_comparable = true;
    };

  }

  template <typename T, std::size_t dim>
  using vec = internal::vec_base<T, dim, generic_vector_tag>;

  typedef vec<unsigned char, 1>	vec1ub;
  typedef vec<unsigned char, 2>	vec2ub;
  typedef vec<unsigned char, 3>	vec3ub;
  typedef vec<char, 1>		vec1b;
  typedef vec<char, 2>		vec2b;
  typedef vec<char, 3>		vec3b;
  typedef vec<unsigned short, 1> vec1us;
  typedef vec<unsigned short, 2> vec2us;
  typedef vec<unsigned short, 3> vec3us;
  typedef vec<short, 1>		vec1s;
  typedef vec<short, 2>		vec2s;
  typedef vec<short, 3>		vec3s;
  typedef vec<unsigned, 1>	vec1u;
  typedef vec<unsigned, 2>	vec2u;
  typedef vec<unsigned, 3>	vec3u;
  typedef vec<int, 1>		vec1i;
  typedef vec<int, 2>		vec2i;
  typedef vec<int, 3>		vec3i;
  typedef vec<float, 1>		vec1f;
  typedef vec<float, 2>		vec2f;
  typedef vec<float, 3>		vec3f;
}

/*********************************/
/**          Traits              */
/*********************************/

namespace std
{

  template <typename T, unsigned dim, typename tag>
  struct is_signed< mln::internal::vec_base<T, dim, tag> > : is_signed<T>
  {
  };

  template <typename T, unsigned dim, typename tag>
  struct is_unsigned< mln::internal::vec_base<T, dim, tag> > : is_unsigned<T>
  {
  };

  template <typename T, unsigned dim, typename tag>
  struct make_signed< mln::internal::vec_base<T, dim, tag> >
  {
    typedef mln::internal::vec_base< typename make_signed<T>::type, dim, tag > type;
  };

  template <typename T, unsigned dim, typename tag>
  struct make_unsigned< mln::internal::vec_base<T, dim, tag> >
  {
    typedef mln::internal::vec_base< typename make_unsigned<T>::type, dim, tag > type;
  };

}

# ifdef _MSC_VER
#	pragma warning(default: 4701)
# endif

#endif
