#pragma once
#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/utility/functional.hpp>
/// \file

namespace mln
{
  /// \brief Apply a function on the values of an image.
  template <class InputImage, class UnaryFunction>
  void for_each(InputImage in, UnaryFunction f);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/
  template <class InputImage, class UnaryFunction>
  void for_each(InputImage input, UnaryFunction f)
  {
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(::ranges::Invocable<UnaryFunction, image_reference_t<InputImage>>());

    auto&& vals = input.new_values();
    for (auto r : ranges::rows(vals))
      ::ranges::for_each(r, f);
  }

} // namespace mln
