#pragma once

#include <mln/core/assert.hpp>
#include <mln/core/config.hpp>
#include <mln/core/literal/vectorial.hpp>

#include <iostream>
#include <type_traits>

// Element-wise operator
// Ex: vector u,v,w;
// w = u + v; <=> w[i] = u[i] + v[i] forall i
#define VEC_BASE_GEN_EW_OP(Trait, Op)                                                                                  \
  template <typename U>                                                                                                \
  requires (vec_base_traits<tag>::Trait)                                                                               \
  vec_base& operator Op(const vec_base<U, dim, tag>& o)                                                                \
  {                                                                                                                    \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      v_[i] Op o.v_[i];                                                                                                \
    return *this;                                                                                                      \
  }

// External operator
// Ex: vector u, v; scalar a;
// u = v + a; <=> u[i] = v[i] + a forall i
#define VEC_BASE_GEN_EXT_OP(Trait, Op)                                                                                 \
  template <typename U>                                                                                                \
  requires(vec_base_traits<tag>::Trait)                                                                                \
  vec_base& operator Op(const U& o)                                                                                    \
  {                                                                                                                    \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      v_[i] Op o;                                                                                                      \
    return *this;                                                                                                      \
  }

// Relational operator
// Ex: vector u, v;
// u < v iif u[i] = v[i] forall i
#define VEC_BASE_GEN_REL(Trait, Op)                                                                                    \
  template <typename U>                                                                                                \
  requires(vec_base_traits<tag>::Trait)                                                                                \
  bool operator Op(const vec_base<U, dim, tag>& o) const                                                               \
  {                                                                                                                    \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      if (!(v_[i] Op o.v_[i]))                                                                                         \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }

// Relational operator
// Ex: vector u, v;
// u op v iif u[i] op v[i] forall i
#define VEC_BASE_GEN_REL_ALL(Trait, Op)                                                                                \
  template <typename U>                                                                                                \
  requires(vec_base_traits<tag>::Trait)                                                                                \
  bool operator Op(const vec_base<U, dim, tag>& o) const                                                               \
  {                                                                                                                    \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      if (!(v_[i] Op o[i]))                                                                                            \
        return false;                                                                                                  \
    return true;                                                                                                       \
  }

// Relational operator
// Ex: vector u, v;
// u op v iif exists i u[i] op v[i]
#define VEC_BASE_GEN_REL_ANY(Trait, Op)                                                                                \
  template <typename U>                                                                                                \
  requires(vec_base_traits<tag>::Trait)                                                                                \
  bool operator Op(const vec_base<U, dim, tag>& o) const                                                               \
  {                                                                                                                    \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      if (v_[i] Op o[i])                                                                                               \
        return true;                                                                                                   \
    return false;                                                                                                      \
  }

// Outside class, Element-wise operator
#define VEC_BASE_GEN_EW_OP_EXT(TraitName, Op)                                                                          \
  template <typename T, typename U, unsigned dim, typename tag>                                                        \
  requires(vec_base_traits<tag>::TraitName)                                                                            \
  vec_base<decltype(std::declval<T>() + std::declval<U>()), dim, tag>inline                                            \
      operator Op(const vec_base<T, dim, tag>& x, const vec_base<U, dim, tag>& y)                                      \
  {                                                                                                                    \
    using R = vec_base<decltype(std::declval<T>() + std::declval<U>()), dim, tag>;                                     \
    R                                                                           r;                                     \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      r[i] = x[i] Op y[i];                                                                                             \
    return r;                                                                                                          \
  }

// Outside class, Relational operator
#define VEC_BASE_GEN_EXT_OP_EXT(TraitName, Op)                                                                         \
  template <typename T, typename U, unsigned dim, typename tag>                                                        \
  requires(vec_base_traits<tag>::TraitName && std::is_convertible_v<U, T>)                                             \
  vec_base_helper_t<std::common_type<T, U>, dim, tag> inline                                                           \
      operator Op(const vec_base<T, dim, tag>& x, const U& y)                                                          \
  {                                                                                                                    \
    using R = vec_base<decltype(std::declval<T>() + std::declval<U>()), dim, tag>;                                     \
    R                                                                           r;                                     \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      r[i] = x[i] Op y;                                                                                                \
    return r;                                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  template <typename T, typename U, unsigned dim, typename tag>                                                        \
  requires(vec_base_traits<tag>::TraitName && std::is_convertible<U, T>::value)                                        \
  vec_base_helper_t<std::common_type<T, U>, dim, tag> inline                                                           \
      operator Op(const U& y, const vec_base<T, dim, tag>& x)                                                          \
  {                                                                                                                    \
    typedef vec_base<decltype(std::declval<T>() + std::declval<U>()), dim, tag> R;                                     \
    R                                                                           r;                                     \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      r[i] = y Op x[i];                                                                                                \
    return r;                                                                                                          \
  }

namespace mln
{

  struct generic_vector_tag
  {
  };

  namespace internal
  {

    template <typename Tag>
    struct vec_base_traits;

    template <typename T, unsigned dim, typename tag = void>
    struct vec_base;

    template <typename T, unsigned dim, typename tag>
    struct vec_base_helper
    {
      using type = vec_base<typename T::type, dim, tag>;
    };

    template <typename T, unsigned dim, typename tag>
    using vec_base_helper_t = typename vec_base_helper<T, dim, tag>::type;

    template <typename T, unsigned dim, typename tag>
    struct vec_base
    {
      template <typename, unsigned, typename>
      friend struct vec_base;

    public:
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator = T*;
      using const_iterator = const T*;
      using reverse_iterator = std::reverse_iterator<T*>;
      using const_reverse_iterator = std::reverse_iterator<const T*>;
      using size_type = size_t;
      using difference_type = std::ptrdiff_t;

      static constexpr unsigned ndim = dim;

      vec_base() = default;

      // Defaulted
      // vec_base(const vec_base&) = default;

      template <class OtherTag>
      explicit vec_base(const vec_base<T, dim, OtherTag>& other)
      {
        for (unsigned i = 0; i < dim; ++i)
          v_[i] = other.v_[i];
      }

      constexpr vec_base(const literal::zero_t&)
        : v_{
              0,
          }
      {
      }

      constexpr vec_base(const literal::one_t&)
        : vec_base(1)
      {
      }

      // explicit
      constexpr vec_base(const T& x)
      {
        for (unsigned i = 0; i < dim; i++)
          v_[i] = x;
      }

      constexpr vec_base(const T& x, const T& y) requires (dim == 2)
      {
        v_[0] = x;
        v_[1] = y;
      }

      constexpr vec_base(const T& x, const T& y, const T& z) requires (dim == 3)
      {
        v_[0] = x;
        v_[1] = y;
        v_[2] = z;
      }

      template <typename U>
      vec_base(const vec_base<U, dim, tag>& other)
      {
        // std::copy(other.v_, other.v_ + dim, v_);
        for (unsigned i = 0; i < dim; ++i)
          v_[i] = static_cast<T>(other.v_[i]);
      }

      template <typename U, typename other_tag>
      explicit vec_base(const vec_base<U, dim, other_tag>& other)
      {
        for (unsigned i = 0; i < dim; ++i)
          v_[i] = static_cast<T>(other.v_[i]);
      }

      void set_all(const T& v) { std::fill(v_, v_ + dim, v); }

      template <typename U>
      vec_base& operator=(const vec_base<U, dim, tag>& other)
      {
        std::copy(other.v_, other.v_ + dim, v_);
        return *this;
      }

      T& operator[](size_type n)
      {
        mln_precondition(n < dim);
        return v_[n];
      }

      constexpr const T& operator[](size_type n) const
      {
        // mln_precondition(n < dim);
        return v_[n];
      }

      vec_base<T, dim, generic_vector_tag>& as_vec()
      {
        return *reinterpret_cast<vec_base<T, dim, generic_vector_tag>*>(this);
      }

      const vec_base<T, dim, generic_vector_tag>& as_vec() const
      {
        return *reinterpret_cast<const vec_base<T, dim, generic_vector_tag>*>(this);
      }

      T*       begin() { return v_; }
      T*       end() { return v_ + dim; }
      const T* begin() const { return v_; }
      const T* end() const { return v_ + dim; }

      constexpr const T& back() const { return v_[dim - 1]; }
      constexpr T&       back() { return v_[dim - 1]; }

      constexpr const T& front() const { return v_[0]; }
      constexpr T&       front() { return v_[0]; }

      reverse_iterator       rbegin() { return reverse_iterator(v_ + dim); }
      reverse_iterator       rend() { return reverse_iterator(v_); }
      const_reverse_iterator rbegin() const { return const_reverse_iterator(v_ + dim); }
      const_reverse_iterator rend() const { return const_reverse_iterator(v_); }

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

      vec_base<decltype(+std::declval<T>()), dim, tag> operator-() const
      {
        typedef vec_base<decltype(+std::declval<T>()), dim, tag> R;
        R                                                        out;
        for (unsigned i = 0; i < dim; ++i)
          out.v_[i] = -v_[i];
        return out;
      }

      /* RELATIONAL */
      template <typename U>
      requires(vec_base_traits<tag>::is_equality_comparable && std::is_convertible_v<T, U>)
      bool operator==(const vec_base<U, dim, tag>& o) const
      {
        for (unsigned i = 0; i < dim; ++i)
          if (!(v_[i] == o[i]))
            return false;
        return true;
      }

      template <typename U>
      requires(vec_base_traits<tag>::is_equality_comparable && std::is_convertible_v<T, U>)
      bool operator!=(const vec_base<U, dim, tag>& o) const
      {
        for (unsigned i = 0; i < dim; ++i)
          if (v_[i] != o[i])
            return true;
        return false;
      }

      template <typename U>
      requires(vec_base_traits<tag>::is_less_than_comparable && std::is_convertible_v<T, U>)
      bool operator<(const vec_base<U, dim, tag>& o) const
      {
        for (unsigned i = 0; i < dim; ++i)
          if (v_[i] < o[i])
            return true;
          else if (v_[i] > o[i])
            return false;
        return false;
      }

      template <typename U>
      requires(vec_base_traits<tag>::is_less_than_comparable && std::is_convertible_v<T, U>)
      bool operator<=(const vec_base<U, dim, tag>& o) const
      {
        for (unsigned i = 0; i < dim; ++i)
          if (v_[i] < o.v_[i])
            return true;
          else if (v_[i] > o[i])
            return false;
        return true;
      }

      template <typename U>
      requires(vec_base_traits<tag>::is_less_than_comparable && std::is_convertible_v<T, U>)
      bool operator>(const vec_base<U, dim, tag>& o) const
      {
        return not(*this <= o);
      }

      template <typename U>
      requires(vec_base_traits<tag>::is_less_than_comparable && std::is_convertible_v<T, U>)
      bool operator>=(const vec_base<U, dim, tag>& o) const
      {
        return not(*this < o);
      }

      template <typename OtherTag>
      operator vec_base<T, dim, OtherTag>&()
      {
        static_assert(std::is_same<tag, generic_vector_tag>::value,
                      "Only a generic a vector can be implicitely converted"
                      "to another vector type. Use as_vec() method.");
        return *reinterpret_cast<vec_base<T, dim, OtherTag>*>(this);
      }

      template <typename OtherTag>
      operator const vec_base<T, dim, OtherTag>&() const
      {
        static_assert(std::is_same<tag, generic_vector_tag>::value,
                      "Only a generic a vector can be implicitely converted"
                      "to another vector type. Use as_vec() method.");
        return *reinterpret_cast<const vec_base<T, dim, OtherTag>*>(this);
      }

      template <typename SumType = T>
      SumType sum(SumType s = 0) const
      {
        for (unsigned i = 0; i < dim; ++i)
          s += v_[i];
        return s;
      }

      template <unsigned N>
      constexpr const T& get() const
      {
        return v_[N];
      }

      template <unsigned N>
      constexpr T& get()
      {
        return v_[N];
      }

    private:
      template <size_t N, class T2, unsigned dim2, typename tag2>
      friend constexpr const T2& get(const vec_base<T2, dim2, tag2>&);
      template <size_t N, class T2, unsigned dim2, typename tag2>
      friend constexpr T2& get(vec_base<T2, dim2, tag2>&);
      template <size_t N, class T2, unsigned dim2, typename tag2>
      friend constexpr T2&& get(vec_base<T2, dim2, tag2>&&);

      T v_[dim] = {};
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

    template <>
    struct vec_base_traits<generic_vector_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = true;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = true;
      static const bool is_equality_comparable  = true;
    };

    template <size_t N, class T, unsigned dim, typename tag>
    inline constexpr T& get(mln::internal::vec_base<T, dim, tag>& vec)
    {
      return vec.v_[N];
    }

    template <size_t N, class T, unsigned dim, typename tag>
    inline constexpr const T& get(const mln::internal::vec_base<T, dim, tag>& vec)
    {
      return vec.v_[N];
    }

    template <size_t N, class T, unsigned dim, typename tag>
    inline constexpr T&& get(mln::internal::vec_base<T, dim, tag>&& vec)
    {
      return std::move<T&>(vec.v_[N]);
    }
  } // namespace internal
} // namespace mln

/*********************************/
/**          Traits              */
/*********************************/

namespace std
{

  template <typename T, unsigned dim, typename tag>
  struct is_signed<mln::internal::vec_base<T, dim, tag>> : is_signed<T>
  {
  };

  template <typename T, unsigned dim, typename tag>
  struct is_unsigned<mln::internal::vec_base<T, dim, tag>> : is_unsigned<T>
  {
  };

  template <typename T, unsigned dim, typename tag>
  struct make_signed<mln::internal::vec_base<T, dim, tag>>
  {
    typedef mln::internal::vec_base<typename make_signed<T>::type, dim, tag> type;
  };

  template <typename T, unsigned dim, typename tag>
  struct make_unsigned<mln::internal::vec_base<T, dim, tag>>
  {
    typedef mln::internal::vec_base<typename make_unsigned<T>::type, dim, tag> type;
  };

  template <typename T1, typename T2, unsigned dim, typename Tag>
  struct common_type<mln::internal::vec_base<T1, dim, Tag>, mln::internal::vec_base<T2, dim, Tag>>
  {
    typedef mln::internal::vec_base<typename std::common_type<T1, T2>::type, dim, Tag> type;
  };
} // namespace std
