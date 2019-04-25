#include <mln/core/image/image.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>

#include <type_traits>


#define MLN_PRIVATE_DEFINE_UNARY_OPERATOR(op, f)                                                                       \
  template <class I, class = std::enable_if_t<::mln::is_a<I, ::mln::experimental::Image>::value>>                      \
  auto op(const I& ima)                                                                                                \
  {                                                                                                                    \
    return ::mln::view::transform(static_cast<const I&>(ima), f);                                                      \
  }

#define MLN_PRIVATE_DEFINE_BINARY_OPERATOR(op, f)                                                                      \
  namespace impl                                                                                                       \
  {                                                                                                                    \
    template <class I1, class I2>                                                                                      \
    auto op(const ::mln::experimental::Image<I1>& ima1, const ::mln::experimental::Image<I2>& ima2)                    \
    {                                                                                                                  \
      return ::mln::view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2), f);                    \
    }                                                                                                                  \
                                                                                                                       \
    template <class I, class Scalar,                                                                                   \
              class = std::enable_if_t<!::mln::is_a<Scalar, ::mln::experimental::Image>::value>>                       \
    auto op(const ::mln::experimental::Image<I>& ima1, Scalar s)                                                       \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = s](auto&& arg) { return f_(arg, s_); };                                                   \
      return ::mln::view::transform(static_cast<const I&>(ima1), g);                                                   \
    }                                                                                                                  \
                                                                                                                       \
    template <class Scalar, class I,                                                                                   \
              class = std::enable_if_t<!::mln::is_a<Scalar, ::mln::experimental::Image>::value>>                       \
    auto op(Scalar s, const ::mln::experimental::Image<I>& ima2)                                                       \
    {                                                                                                                  \
      auto g = [f_ = f, s_ = s](auto&& arg) { return f_(s_, arg); };                                                   \
      return ::mln::view::transform(static_cast<const I&>(ima2), g);                                                   \
    }                                                                                                                  \
  } /* namespace impl */                                                                                               \
                                                                                                                       \
  /* This overload is there to be a best match wrt old API impl */                                                     \
  template <class A, class B,                                                                                          \
            class = std::enable_if_t<(::mln::is_a<A, ::mln::experimental::Image>::value ||                             \
                                      ::mln::is_a<B, ::mln::experimental::Image>::value)>>                             \
  auto op(const A& lhs, const B& rhs)                                                                                  \
  {                                                                                                                    \
    return impl::op(lhs, rhs);                                                                                         \
  }
