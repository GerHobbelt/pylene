#pragma once

#include <mln/core/forall.hpp>
#include <mln/core/range/range.hpp>


namespace mln
{

  namespace range
  {

    template <class InputRange, class Function>
    Function for_each(InputRange&& rng, Function f)
    {
      mln_iter(v, rng);
      mln_forall (v)
        f(*v);
      return f;
    }
  } // namespace range
} // namespace mln
