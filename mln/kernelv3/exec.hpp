#ifndef MLN_KERNEL_EXEC_HPP
# define MLN_KERNEL_EXEC_HPP

# include <mln/kernelv3/expr/expr.hpp>
# include <mln/kernelv3/internal/execute_linear.hpp>

namespace mln
{
  namespace kernel
  {

    /// \brief Execute a linear expression
    /// \param expr The expression to execute
    template <class E>
    void
    exec(const Expr<E>& expr);


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class E>
    void
    exec(const Expr<E>& expr)
    {
      return internal::execute_linear(expr);
    }


  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNEL_EXEC_HPP
