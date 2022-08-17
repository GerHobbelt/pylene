#include <ano/core/image/image.hpp>
#include <ano/core/image/view/transform.hpp>

#include <type_traits>


#define ANO_PRIVATE_DEFINE_UNARY_OPERATOR(op, f)                                                                       \
  template <class I>                                                                                                   \
  requires(::ano::is_a<I, ::ano::details::Image>::value) auto op(const I& ima)                                         \
  {                                                                                                                    \
    return ::ano::view::transform(ima, f);                                                                             \
  }

#define ANO_PRIVATE_DEFINE_BINARY_OPERATOR(op, f)                                                                      \
  template <class A, class B>                                                                                          \
  requires(::ano::is_a<A, ::ano::details::Image>::value || ::ano::is_a<B, ::ano::details::Image>::value) auto op(      \
      const A& a, const B& b)                                                                                          \
  {                                                                                                                    \
    constexpr bool is_A_image = ::ano::is_a<A, ::ano::details::Image>::value;                                          \
    constexpr bool is_B_image = ::ano::is_a<B, ::ano::details::Image>::value;                                          \
                                                                                                                       \
    if constexpr (is_A_image && is_B_image)                                                                            \
      return ::ano::view::transform(a, b, f);                                                                          \
    else if constexpr (is_A_image)                                                                                     \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = b](auto&& arg) { return f_(arg, s_); };                                                   \
      return ::ano::view::transform(a, g);                                                                             \
    }                                                                                                                  \
    else if constexpr (is_B_image)                                                                                     \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = a](auto&& arg) { return f_(s_, arg); };                                                   \
      return ::ano::view::transform(b, g);                                                                             \
    }                                                                                                                  \
  }
