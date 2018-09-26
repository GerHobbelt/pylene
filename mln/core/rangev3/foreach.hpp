#pragma once

#include <mln/core/rangev3/rows.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>



/// \brief The macro defines mln_foreach(RANGE_DECLARATION, RANGE_EXPRESSION) as equivalent to:
///
/// \code
/// for (auto&& row : mln::ranges::rows(RANGE_EXPRESSION))
///   for (RANGE_DECLARATION : row)
/// \endcode
///
/// with support for *break* statement.
///
/// \note Due to limited support for comma by preprocessor, any RANGE_DECLARATION with comma must be enclosed
/// in parenthesis. For example:
///
/// \code
///   mln_foreach((auto [a,b]), multispan)
/// \endcode
#define mln_foreach_new(PROTECTED_DECL, RNG)                                                                           \
  if (bool __mln_has_been_broken = false; false)                                                                       \
  {                                                                                                                    \
  }                                                                                                                    \
  else                                                                                                                 \
    for (auto&& __mln_inner_rng : mln::ranges::rows(RNG))               \
      if (__mln_has_been_broken)                                                                                       \
        break;                                                                                                         \
      else                                                                                                             \
        for (BOOST_PP_REMOVE_PARENS(PROTECTED_DECL) : __mln_inner_rng)                                                 \
          if (__mln_has_been_broken)                                                                                   \
            break;                                                                                                     \
          else                                                                                                         \
            for (__mln_has_been_broken = true; __mln_has_been_broken; __mln_has_been_broken = false)
