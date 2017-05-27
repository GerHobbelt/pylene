#ifndef MLN_KERNELV3_EXPR_TERMINAL_HPP
# define MLN_KERNELV3_EXPR_TERMINAL_HPP

namespace mln
{

  namespace kernel
  {

    template <class T>
    struct terminal_expr : expr< tag::terminal, terminal_expr<T> >
    {
      terminal_expr(T x) : value(x) {}
      T value;
    };

    template <class T>
    terminal_expr<T> make_terminal(T x)
    {
      return {x};
    }

  } // end of namespace mln::kernel

} // end of namespace mln

#endif //!MLN_KERNELV3_EXPR_TERMINAL_HPP
