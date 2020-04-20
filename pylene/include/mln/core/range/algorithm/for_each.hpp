#pragma once

#include <mln/core/concepts/range.hpp>
#include <range/v3/algorithm/for_each.hpp>


namespace mln::ranges
{

  /// \brief Apply a function on each element of a range
  ///
  ///
  /// \param rng range
  /// \param fun function to apply
  ///
  /// \tparam Rng must be a range
  /// \tparam F   must be invocable with range value type
  ///
  ///
  template <MDRange Rng, class F>
  void for_each(Rng&& rng, F fun);


  /******************************************/
  /****          implementation          ****/
  /******************************************/

  template <MDRange Rng, class F>
  void for_each(Rng&& rng, F fun)
  {
    for (auto&& r : rng.rows())
      ::ranges::for_each(r, fun);
  }


  template <::ranges::cpp20::range Rng, class F>
  void for_each(Rng&& rng, F fun)
  {
    ::ranges::for_each(std::forward<Rng>(rng), fun);
  }

}
