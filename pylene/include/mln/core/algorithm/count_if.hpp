#pragma once

#include <mln/core/concept/new/images.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/count_if.hpp>


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
    static_assert(::ranges::Predicate<UnaryPredicate, image_reference_t<InputImage>>());

    auto&&         vals = input.new_values();
    std::ptrdiff_t k    = 0;

    for (auto r : ranges::rows(vals))
      k += ::ranges::count_if(r, p);

    return k;
  }
} // namespace mln
