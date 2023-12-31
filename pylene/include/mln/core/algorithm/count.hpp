#pragma once

#include <mln/core/concepts/image.hpp>

#include <mln/core/range/rows.hpp>
#include <range/v3/algorithm/count.hpp>


namespace mln
{


  template <class InputImage, class Value>
  std::ptrdiff_t count(InputImage input, const Value& v);


  /******************/
  /* Implem         */
  /******************/

  template <class InputImage, class Value>
  std::ptrdiff_t count(InputImage input, const Value& val)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>());

    auto&&         vals = input.values();
    std::ptrdiff_t k    = 0;

    for (auto r : ranges::rows(vals))
      k += ::ranges::count(r, val);

    return k;
  }
} // namespace mln
