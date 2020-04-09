#pragma once

#include <mln/core/concepts/image.hpp>
#include <mln/core/range/rows.hpp>

#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/functional/concepts.hpp>

namespace mln
{

  template <class InputImage, class UnaryPredicate>
  std::ptrdiff_t count_if(InputImage input, UnaryPredicate p);

  /******************/
  /* Implem         */
  /******************/

  template <class InputImage, class UnaryPredicate>
  std::ptrdiff_t count_if(InputImage input, UnaryPredicate p)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::predicate<UnaryPredicate, image_reference_t<InputImage>>);

    auto&&         vals = input.new_values();
    std::ptrdiff_t k    = 0;

    for (auto r : ranges::rows(vals))
      k += ::ranges::count_if(r, p);

    return k;
  }
} // namespace mln
