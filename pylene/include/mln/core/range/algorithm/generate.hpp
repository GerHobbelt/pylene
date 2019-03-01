#pragma once

#include <mln/core/forall.hpp>
#include <mln/core/range/range.hpp>


namespace mln
{
  namespace range
  {

    template <class Range, class Generator>
    void generate(Range&& rng, Generator f)
    {
      mln_iter(vin, rng);
      mln_forall (vin)
        *vin = f();
    }

  } // namespace range

} // namespace mln
