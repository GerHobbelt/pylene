#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/count.hpp>


namespace mln
{

  namespace experimental
  {
    template <class InputImage, class Value>
    auto count(InputImage input, const Value& v);
  }

  /******************/
  /* Implem         */
  /******************/

  namespace experimental
  {
    template <class InputImage, class Value>
    auto count(InputImage input, const Value& v)
    {
      static_assert(mln::is_a<InputImage, Image>());

      auto&&         vals = input.new_values();
      std::ptrdiff_t cmpt = 0;

      for (auto r : ranges::rows(vals))
        cmpt += ::ranges::count(r, v);

      return cmpt;
    }
  } // namespace experimental
} // namespace mln
