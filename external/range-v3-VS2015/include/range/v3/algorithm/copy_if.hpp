/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_COPY_IF_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct copy_if_fn
        {
            template<typename I, typename S, typename O, typename F, typename P = ident,
#ifdef RANGES_WORKAROUND_MSVC_SFINAE_CONSTEXPR
                CONCEPT_REQUIRES_(InputIterator<I>::value && IteratorRange<I, S>::value &&
                    WeaklyIncrementable<O>::value && IndirectCallablePredicate<F, Projected<I, P> >::value &&
                    IndirectlyCopyable<I, O>::value)>
#else
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() &&
                    WeaklyIncrementable<O>() && IndirectCallablePredicate<F, Projected<I, P> >() &&
                    IndirectlyCopyable<I, O>())>
#endif
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O out, F pred_, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                for(; begin != end; ++begin)
                {
                    auto &&x = *begin;
                    if(pred(proj(x)))
                    {
                        *out = (decltype(x) &&) x;
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
#ifdef RANGES_WORKAROUND_MSVC_SFINAE_CONSTEXPR
                CONCEPT_REQUIRES_(InputRange<Rng>::value && WeaklyIncrementable<O>::value &&
                    IndirectCallablePredicate<F, Projected<I, P> >::value && IndirectlyCopyable<I, O>::value)>
#else
                CONCEPT_REQUIRES_(InputRange<Rng>() && WeaklyIncrementable<O>() &&
                    IndirectCallablePredicate<F, Projected<I, P> >() && IndirectlyCopyable<I, O>())>
#endif
            tagged_pair<tag::in(range_safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, O out, F pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), std::move(proj));
            }
        };

        /// \sa `copy_if_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& copy_if = static_const<with_braced_init_args<copy_if_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
