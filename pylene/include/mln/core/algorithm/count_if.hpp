#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/count_if.hpp>


namespace mln
{

  namespace experimental
  {
    template <class InputImage, class UnaryPredicate>
    auto count_if(InputImage input, UnaryPredicate p);
  }

  /******************/
  /* Implem         */
  /******************/

  namespace experimental
  {
    template <class InputImage, class UnaryPredicate>
    auto count_if(InputImage input, UnaryPredicate p)
    {
      static_assert(mln::is_a<InputImage, Image>());
      static_assert(::ranges::Predicate<UnaryPredicate, image_reference_t<InputImage>>());

      auto&&         vals = input.new_values();
      std::ptrdiff_t k    = 0;

      for (auto r : ranges::rows(vals))
        k += ::ranges::count_if(r, std::move(p));

      return k;
    }
  } // namespace experimental
} // namespace mln
