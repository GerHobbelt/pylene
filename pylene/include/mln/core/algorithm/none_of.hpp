#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/utility/functional.hpp>


namespace mln
{

  template <class InputImage, class UnaryPredicate>
  bool none_of(InputImage input, UnaryPredicate p);

  template <class InputImage>
  bool none_of(InputImage input);

  /******************/
  /* Implem         */
  /******************/

  template <class InputImage, class UnaryPredicate>
  bool none_of(InputImage input, UnaryPredicate p)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::Predicate<UnaryPredicate, image_reference_t<InputImage>>());

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      if (!::ranges::none_of(r, p))
        return false;
    return true;
  }

  template <class InputImage>
  bool none_of(InputImage input)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(std::is_convertible_v<image_reference_t<InputImage>, bool>);

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      for (auto&& v : r)
        if (v)
          return false;
    return true;
  }

} // namespace mln
