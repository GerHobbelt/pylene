#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/utility/functional.hpp>

namespace mln
{

  namespace experimental
  {
    template <class InputImage, class UnaryPredicate>
    bool any_of(InputImage input, UnaryPredicate p);

    template <class InputImage>
    bool any_of(InputImage input);
  }

  /******************/
  /* Implem         */
  /******************/

  namespace experimental
  {
    template <class InputImage, class UnaryPredicate>
    bool any_of(InputImage input, UnaryPredicate p)
    {
      static_assert(mln::is_a<InputImage, Image>());
      static_assert(::ranges::Predicate<UnaryPredicate, image_reference_t<InputImage>>());

      for (auto r : ranges::rows(input.new_values()))
        if (::ranges::any_of(r, p))
          return true;
      return false;
    }

    template <class InputImage>
    bool any_of(InputImage input)
    {
      static_assert(mln::is_a<InputImage, Image>());
      static_assert(std::is_convertible_v<image_reference_t<InputImage>, bool>);

      for (auto r : ranges::rows(input.new_values()))
        for (auto&& v : r)
          if (v)
            return true;
      return false;
    }

  } // namespace experimental
} // namespace mln
