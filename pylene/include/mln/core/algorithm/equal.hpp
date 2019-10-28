#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <range/v3/algorithm/equal.hpp>
#include <concepts/concepts.hpp>


namespace mln
{

  /// \brief Compare if two image are equals.
  ///
  /// Two image are said equal if they have the same values.
  ///
  /// \param lhs First image
  /// \param rhs Second image
  ///
  /// \tparam LhsImage must model a Readable Forward Image
  /// \tparam RhsImage must model a Readable Forward Image
  ///
  /// \return True if image are equals.
  ///
  template <class LhsImage, class RhsImage>
  bool equal(LhsImage lhs, RhsImage rhs);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class LhsImage, class RhsImage>
  bool equal(LhsImage lhs, RhsImage rhs)
  {
    static_assert(mln::is_a<LhsImage, experimental::Image>());
    static_assert(mln::is_a<RhsImage, experimental::Image>());
    static_assert(::concepts::equality_comparable_with<image_value_t<LhsImage>, image_value_t<RhsImage>>);

    auto&& lhs_vals = lhs.new_values();
    auto&& rhs_vals = rhs.new_values();
    auto r1 = ranges::rows(lhs_vals);
    auto r2 = ranges::rows(rhs_vals);

    for (auto [lhs_r, rhs_r] : ranges::view::zip(r1, r2))
      // FIXME: with std::equal you gain performances over ranges::equal here
      if (!::ranges::equal(lhs_r, rhs_r))
        return false;

    return true;
  }

} // namespace mln
