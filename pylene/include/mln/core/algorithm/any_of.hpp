#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/range/rows.hpp>

#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/functional/concepts.hpp>


namespace mln
{
  template <class InputImage, class UnaryPredicate>
  bool any_of(InputImage input, UnaryPredicate p);

  template <class InputImage>
  bool any_of(InputImage input);

  /******************/
  /* Implem         */
  /******************/

  template <class InputImage, class UnaryPredicate>
  bool any_of(InputImage input, UnaryPredicate p)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::cpp20::predicate<UnaryPredicate, image_reference_t<InputImage>>);

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      if (::ranges::any_of(r, p))
        return true;
    return false;
  }

  template <class InputImage>
  bool any_of(InputImage input)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(std::is_convertible_v<image_reference_t<InputImage>, bool>);

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      for (auto&& v : r)
        if (v)
          return true;
    return false;

  } // namespace experimental
} // namespace mln
