#pragma once

#include <mln/core/rangev3/view/remove_if.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace mln::ranges::view
{
  /// Given a source range and a unary predicate, filter the elements
  /// that satisfy the predicate.
  struct filter_fn
  {
    template <typename Rng, typename Pred>
    remove_if_view<::ranges::view::all_t<Rng>, ::ranges::logical_negate<Pred>> operator()(Rng&& rng, Pred pred) const
    {
      CONCEPT_ASSERT(::ranges::Range<Rng>());
      CONCEPT_ASSERT(::ranges::IndirectPredicate<Pred, ::ranges::iterator_t<Rng>>());
      return {::ranges::view::all(static_cast<Rng&&>(rng)), ::ranges::not_fn(std::move(pred))};
    }

    template <typename Pred>
    auto operator()(Pred pred) const
    {
      return ::ranges::make_pipeable(std::bind(*this, std::placeholders::_1, ::ranges::protect(std::move(pred))));
    }
  };

  /// \relates filter_fn
  /// \ingroup group-views
  RANGES_INLINE_VARIABLE(filter_fn, filter)
} // namespace mln::ranges::view
