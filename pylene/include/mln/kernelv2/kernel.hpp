#pragma once

#include <mln/core/config.hpp>
#include <mln/kernelv2/details/deep_copy_fix.hpp>
#include <mln/kernelv2/execute.hpp>
#include <mln/kernelv2/execute_incremental.hpp>
#include <mln/kernelv2/types.hpp>


namespace mln
{

  namespace kernel
  {

    /// \brief Declare a kernel expression
    template <class Expr>
    typename proto::result_of::deep_copy<Expr>::type declare(Expr&& e);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class Expr>
    typename proto::result_of::deep_copy<Expr>::type declare(Expr&& e)
    {
      return proto::deep_copy(std::forward<Expr>(e));
    }

  } // namespace kernel

} // namespace mln
