#pragma once

#include <mln/accu/accumulators/infsup.hpp>
#include <mln/kernelv2/types.hpp>


namespace mln
{
  namespace kernel
  {
    namespace aggregate
    {

      template <class Compare = void>
      using Inf_t = Aggregate<accu::features::inf<Compare>>;

      template <class Compare = void, class Expr>
      auto Inf(Expr&& expr) -> decltype(Inf_t<Compare>()(std::declval<Expr>()))
      {
        return Inf_t<Compare>()(std::forward<Expr>(expr));
      }

    } // namespace aggregate

  } // namespace kernel

} // namespace mln
