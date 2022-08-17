#pragma once

#include <ano/core/concepts/image.hpp>
#include <ano/core/range/rows.hpp>

#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/functional/concepts.hpp>

namespace ano
{

  template <class InputImage, class UnaryPredicate>
  std::ptrdiff_t count_if(InputImage input, UnaryPredicate p);

  /******************/
  /* Implem         */
  /******************/

  template <class InputImage, class UnaryPredicate>
  std::ptrdiff_t count_if(InputImage input, UnaryPredicate p)
  {
    static_assert(ano::is_a<InputImage, ano::details::Image>());
    static_assert(::ranges::predicate<UnaryPredicate, image_reference_t<InputImage>>);

    auto&&         vals = input.values();
    std::ptrdiff_t k    = 0;

    for (auto r : ranges::rows(vals))
      k += ::ranges::count_if(r, p);

    return k;
  }
} // namespace ano
