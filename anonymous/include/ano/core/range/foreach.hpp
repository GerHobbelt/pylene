#pragma once

#include <ano/core/range/rows.hpp>
#include <boost/preprocessor/punctuation/remove_parens.hpp>


/// \brief The macro defines ano_foreach(RANGE_DECLARATION, RANGE_EXPRESSION) as equivalent to:
///
/// \code
/// for (auto&& row : ano::ranges::rows(RANGE_EXPRESSION))
///   for (RANGE_DECLARATION : row)
/// \endcode
///
/// with support for *break* statement.
///
/// \note Due to limited support for comma by preprocessor, any RANGE_DECLARATION with comma must be enclosed
/// in parenthesis. For example:
///
/// \code
///   ano_foreach((auto [a,b]), multispan)
/// \endcode
#define ano_foreach(PROTECTED_DECL, RNG)                                                                               \
  if (bool __ano_has_been_broken = false; false)                                                                       \
  {                                                                                                                    \
  }                                                                                                                    \
  else if (auto&& __ano_rng = RNG; false)                                                                              \
  {                                                                                                                    \
  }                                                                                                                    \
  else                                                                                                                 \
    for (auto&& __ano_inner_rng : ano::ranges::rows(__ano_rng))                                                        \
      if (__ano_has_been_broken)                                                                                       \
        break;                                                                                                         \
      else                                                                                                             \
        for (BOOST_PP_REMOVE_PARENS(PROTECTED_DECL) : __ano_inner_rng)                                                 \
          if (__ano_has_been_broken)                                                                                   \
            break;                                                                                                     \
          else                                                                                                         \
            for (__ano_has_been_broken = true; __ano_has_been_broken; __ano_has_been_broken = false)
