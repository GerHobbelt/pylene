#pragma once

#include <mln/core/range/private/mdcursor.hpp>
#include <range/v3/algorithm/equal.hpp>


namespace mln::ranges
{

  /// \brief Compare if two image Multidimensional Range are equal.
  ///
  ///
  /// \param rng1 First range
  /// \param rng2 Second range
  ///
  /// \tparam Rng1 must model a Forward MDRange
  /// \tparam Rng2 must model a Forward MDRange
  ///
  /// \return True if image are equals.
  ///
  template <MDRange Rng1, MDRange Rng2>
  bool equal(Rng1&& rng1, Rng2&& rng2);


  /******************************************/
  /****          implementation          ****/
  /******************************************/

  template <MDRange Rng1, MDRange Rng2>
  bool equal(Rng1&& rng1, Rng2&& rng2)
  {
    auto r1 = rng1.rows();
    auto r2 = rng2.rows();

    auto it1 = ::ranges::begin(r1);
    auto it2 = ::ranges::begin(r2);
    auto end = ::ranges::end(r1);

    for (; it1 != end; ++it1, ++it2)
    {
      if (! ::ranges::equal(*it1, *it2))
        return false;
    }
    return true;
  }


}
