#pragma once

#include <mln/core/range/view/remove_if.hpp>
#include <range/v3/view/filter.hpp>

namespace mln::ranges::view
{

  template <MDRange Rng, class Pred>
  [[gnu::always_inline]] inline auto filter(Rng&& rng, Pred pred)
  {
    using R = std::remove_cvref_t<Rng>;
    using P = ::ranges::logical_negate<Pred>;
    P neg   = ::ranges::not_fn(std::move(pred));
    return remove_if_mdview<R, P>{std::forward<Rng>(rng), std::move(neg)};
  }


  template <::ranges::cpp20::range Rng, class Fun>
  [[gnu::always_inline]] inline auto filter(Rng&& rng, Fun&& fun)
  {
    return ::ranges::cpp20::views::filter(std::forward<Rng>(rng), std::forward<Fun>(fun));
  }

} // namespace mln::ranges::view
