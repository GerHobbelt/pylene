#ifndef MLN_KERNELV3_EXPR_OPS_HPP
# define MLN_KERNELV3_EXPR_OPS_HPP

# include "expr.hpp"
# include "terminal.hpp"
# include <tuple>

namespace mln
{
  namespace kernel
  {

    template <class F, class... ArgTypes>
    struct function_expr :
      nary_expr<tag::function, function_expr<F, ArgTypes...>, ArgTypes... >
    {
      using Base = nary_expr<tag::function, function_expr<F, ArgTypes...>, ArgTypes... >;

      function_expr(const F& f_, const boost::hana::tuple<ArgTypes...>& args_)
        : Base(args_), f(f_){}

      F f;
    };


    template <class F, class... ArgTypes>
    function_expr<F, ArgTypes...>
    make_function_expr(const F& f, const ArgTypes&... args)
    {
      return {f, boost::hana::make_tuple(args...)};
    }


    template <class F, class... ArgTypes>
    function_expr<F, ArgTypes...>
    make_function_expr(const F& f, const std::tuple<ArgTypes...>& args)
    {
      return {f, args};
    }

    template <class E1, class E2>
    function_expr<std::plus<>, E1, E2>
    operator+ (const Expr<E1>& lhs, const Expr<E2>& rhs)
    {
      return make_function_expr(std::plus<>(), lhs.exact(), rhs.exact());
    }

    template <class E1, class Scalar>
    std::enable_if_t<not is_an_expr<Scalar>::value,
                     function_expr<std::plus<>, E1, terminal_expr<Scalar> > >
    operator+ (const Expr<E1>& lhs, const Scalar& rhs)
    {
      return make_function_expr(std::plus<>(), lhs.exact(), make_terminal(rhs));
    }

    template <class Scalar, class E2>
    std::enable_if_t<not is_an_expr<Scalar>::value,
                     function_expr<std::plus<>, terminal_expr<Scalar>, E2> >
    operator+ (const Scalar& lhs, const Expr<E2>& rhs)
    {
      return make_function_expr(std::plus<>(), make_terminal(lhs), rhs.exact());
    }





  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNELV3_EXPR_OPS_HPP
