#pragma once

#include <mln/core/image/private/image_traits.hpp>
#include <mln/core/rangev3/view/filter.hpp>

#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>

#include <utility>

namespace mln::detail
{


  template <typename I, typename F>
  struct filtered
  {
    using value_type = image_value_t<I>;
    using reference  = image_reference_t<I>;

    filtered(I ima, F f)
      : rng_(mln::ranges::view::filter(ima.domain(), f))
      , ima_(ima)
      , f_(f)
    {
    }

    auto begin() { return ::ranges::begin(rng_); }
    auto end() { return ::ranges::end(rng_); }

    bool has(value_type p) const { return f_(ima_(p)); }
    bool empty() const { return ::ranges::empty(rng_); }

  private:
    using rng_t = decltype(mln::ranges::view::filter(std::declval<I>().domain(), std::declval<F>()));

    rng_t rng_;
    I     ima_;
    F     f_;
  };

} // namespace mln::detail
