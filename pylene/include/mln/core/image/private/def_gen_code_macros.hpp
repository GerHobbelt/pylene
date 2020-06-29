#include <mln/core/image/image.hpp>
#include <mln/core/image/view/transform.hpp>

#include <type_traits>


#define MLN_PRIVATE_DEFINE_UNARY_OPERATOR(op, f)                                                                       \
  template <class I>                                                                                                   \
  requires(::mln::is_a<I, ::mln::details::Image>::value) auto op(const I& ima)                                         \
  {                                                                                                                    \
    return ::mln::view::transform(ima, f);                                                                             \
  }

#define MLN_PRIVATE_DEFINE_BINARY_OPERATOR(op, f)                                                                      \
  template <class A, class B>                                                                                          \
  requires(::mln::is_a<A, ::mln::details::Image>::value || ::mln::is_a<B, ::mln::details::Image>::value) auto op(      \
      const A& a, const B& b)                                                                                          \
  {                                                                                                                    \
    constexpr bool is_A_image = ::mln::is_a<A, ::mln::details::Image>::value;                                          \
    constexpr bool is_B_image = ::mln::is_a<B, ::mln::details::Image>::value;                                          \
                                                                                                                       \
    if constexpr (is_A_image && is_B_image)                                                                            \
      return ::mln::view::transform(a, b, f);                                                                          \
    else if constexpr (is_A_image)                                                                                     \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = b](auto&& arg) { return f_(arg, s_); };                                                   \
      return ::mln::view::transform(a, g);                                                                             \
    }                                                                                                                  \
    else if constexpr (is_B_image)                                                                                     \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = a](auto&& arg) { return f_(s_, arg); };                                                   \
      return ::mln::view::transform(b, g);                                                                             \
    }                                                                                                                  \
  }
