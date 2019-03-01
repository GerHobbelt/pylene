#pragma once

#include <mln/core/forall.hpp>
#include <mln/core/range/range.hpp>


namespace mln
{
  namespace range
  {

    template <class Range1, class T>
    T accumulate(const Range1& rng, T init)
    {
      mln_iter(vin, rng);
      mln_forall (vin)
        init = init + *vin;
      return init;
    }

    template <class Range1, class T, class BinaryOperation>
    T accumulate(const Range1& rng, T init, BinaryOperation op)
    {
      mln_iter(vin, rng);
      mln_forall (vin)
        init = op(init, *vin);
      return init;
    }

  } // namespace range
} // namespace mln
