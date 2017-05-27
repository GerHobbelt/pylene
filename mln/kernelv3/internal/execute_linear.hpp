#ifndef MLN_KERNEL_INTERNAL_EXECUTE_LINEAR_HPP
# define MLN_KERNEL_INTERNAL_EXECUTE_LINEAR_HPP

# include <mln/kernelv3/expr/expr.hpp>
# include <mln/kernelv3/visitors/evaluator.hpp>
# include <mln/kernelv3/internal/contexts/iterator_context.hpp>

namespace mln
{
  namespace kernel
  {
    namespace internal
    {

      template <class E>
      void
      execute_linear(const Expr<E>& expr)
      {
        auto ctx = contexts::make_iterator_context(expr);
        auto& zip_it = ctx.iter();


        mln_forall(zip_it)
        {
          visitors::Evaluator<decltype(*zip_it)> viz(*zip_it);
          expr.accept(viz);
        }
      };

    } // end of namespace mln::kernel::internal
  } // end of namespace mln::kernel
} // end of namespace mln

#endif //!MLN_KERNEL_INTERNAL_EXECUTE_LINEAR_HPP
