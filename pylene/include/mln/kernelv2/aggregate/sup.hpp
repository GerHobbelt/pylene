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
      using Sup_t = Aggregate<accu::features::sup<Compare>>;

      template <class Compare = void, class Expr>
      auto Sup(Expr&& expr) -> decltype(Sup_t<Compare>()(std::declval<Expr>()))
      {
        return Sup_t<Compare>()(std::forward<Expr>(expr));
      }

    } // namespace aggregate

  } // namespace kernel

} // namespace mln
