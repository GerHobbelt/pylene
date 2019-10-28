#pragma once

#include <range/v3/range/concepts.hpp>
#include <mln/core/rangev3/private/mdcursor.hpp>
#include <mln/core/rangev3/type_traits.hpp>

namespace mln::ranges
{
  template <class R>
  concept mdrange = MDRange<R> || ::ranges::cpp20::range<R>;

  template <class R, class V>
  concept output_mdrange = mdrange<R> && ::ranges::cpp20::output_range<mdrange_row_t<R>, V>;

  template <class R>
  concept reversible_mdrange = MDBidirectionalRange<R> || ::ranges::cpp20::bidirectional_range<R>;

}
