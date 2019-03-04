#pragma once

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

    } // namespace aggregate

  } // namespace kernel

} // namespace mln
