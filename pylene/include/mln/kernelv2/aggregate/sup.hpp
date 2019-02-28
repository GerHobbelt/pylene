#ifndef MLN_KERNEL_AGGREGATE_SUP_HPP
#define MLN_KERNEL_AGGREGATE_SUP_HPP

#include <mln/accu/accumulators/infsup.hpp>
#include <mln/kernelv2/types.hpp>

namespace mln
{
  namespace kernel
  {
    namespace aggregate
    {

      template <class Compare = void>
      using Sup_t = Aggregate<accu::features::sup<Compare>>;

      template <class Compare = void, class Expr>
      auto Sup(Expr&& expr) -> decltype(Sup_t<Compare>()(std::declval<Expr>()))
      {
        return Sup_t<Compare>()(std::forward<Expr>(expr));
      }

    } // namespace mln::kernel::aggregate

  } // namespace mln::kernel

} // namespace mln

#endif //! MLN_KERNEL_AGGREGATE_SUP_HPP
