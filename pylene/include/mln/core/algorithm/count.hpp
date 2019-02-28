#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/count.hpp>


namespace mln
{

  namespace experimental
  {
    template <class InputImage, class Value>
    std::ptrdiff_t count(InputImage input, const Value& v);
  }

  /******************/
  /* Implem         */
  /******************/

  namespace experimental
  {
    template <class InputImage, class Value>
    std::ptrdiff_t count(InputImage input, const Value& v)
    {
      static_assert(mln::is_a<InputImage, Image>());

      auto&&         vals = input.new_values();
      std::ptrdiff_t k    = 0;

      for (auto r : ranges::rows(vals))
        k += ::ranges::count(r, v);

      return k;
    }
  } // namespace experimental
} // namespace mln
