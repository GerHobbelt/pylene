#pragma once

#include <ano/core/range/private/mdcursor.hpp>
#include <ano/core/range/type_traits.hpp>
#include <range/v3/range/concepts.hpp>

namespace ano::ranges
{
  template <class R>
  concept mdrange = MDRange<R> || ::ranges::cpp20::range<R>;

  template <class R, class V>
  concept output_mdrange = mdrange<R> && ::ranges::cpp20::output_range<mdrange_row_t<R>, V>;

  template <class R>
  concept reversible_mdrange = MDBidirectionalRange<R> || ::ranges::cpp20::bidirectional_range<R>;

} // namespace ano::ranges
