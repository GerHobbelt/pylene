#pragma once

#include <mln/core/rangev3/view/remove_if.hpp>

#include <mln/core/concept/new/concepts.hpp>

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
    using Constraint = ::meta::and_<::ranges::InputRange<Rng>, ::ranges::CopyConstructible<Pred>,
                                    ::ranges::Predicate<Pred, ::ranges::reference_t<::ranges::iterator_t<Rng>>>
                                    // FIXME: IndirectPredicate is bugged, see issue #1077
                                    // https://github.com/ericniebler/range-v3/issues/1077
                                    //, ::ranges::IndirectPredicate<Pred, ::ranges::iterator_t<Rng>>
                                    >;

    template <typename Rng, typename Pred, CONCEPT_REQUIRES_(Constraint<Rng, Pred>())>
    // clang-format off
#ifdef PYLENE_CONCEPT_TS_ENABLED
    requires mln::concepts::stl::InputRange<Rng> &&
             mln::concepts::stl::IndirectUnaryPredicate<Pred, ::ranges::iterator_t<Rng>>
#endif
    remove_if_view<::ranges::view::all_t<Rng>, ::ranges::logical_negate<Pred>> operator()(Rng&& rng, Pred pred) const
    // clang-format on
    {
      return {::ranges::view::all(static_cast<Rng&&>(rng)), ::ranges::not_fn(std::move(pred))};
    }

    template <typename Rng, typename Pred, CONCEPT_REQUIRES_(!Constraint<Rng, Pred>())>
    void operator()(Rng&&, Pred) const
    {
      CONCEPT_ASSERT_MSG(::ranges::InputRange<Rng>(), "The first argument to view::remove_if must be a model of the "
                                                      "InputRange concept");
      CONCEPT_ASSERT_MSG(::meta::and_<::ranges::CopyConstructible<Pred>,
                                      ::ranges::Predicate<Pred, ::ranges::reference_t<::ranges::iterator_t<Rng>>>
                                      // FIXME: IndirectPredicate is bugged, see issue #1077
                                      // https://github.com/ericniebler/range-v3/issues/1077
                                      //::ranges::IndirectPredicate<Pred, ::ranges::iterator_t<Rng>
                                      >(),
                         "The second argument to view::remove_if must be callable with "
                         "a value of the range, and the return type must be convertible "
                         "to bool");
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
