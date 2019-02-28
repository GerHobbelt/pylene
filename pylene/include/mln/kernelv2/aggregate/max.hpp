#ifndef MLN_KERNEL_AGGREGATE_MAX_HPP
#define MLN_KERNEL_AGGREGATE_MAX_HPP

#include <mln/accu/accumulators/max.hpp>
#include <mln/kernel/aggregate.hpp>

namespace mln
{
  namespace kernel
  {
    namespace aggregate
    {

      template <class Compare = void>
      using Max_t = Aggregate<accu::features::max<Compare>>;

      template <class Compare = void, class Expr>
      auto Max(Expr&& expr) -> decltype(Max_t<Compare>()(std::declval<Expr>()))
      {
        return Max_t<Compare>()(std::forward<Expr>(expr));
      }

    } // namespace mln::kernel::aggregate

  } // namespace mln::kernel

} // namespace mln

#endif //! MLN_KERNEL_AGGREGATE_MAX_HPP
