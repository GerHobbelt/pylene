#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/range/rows.hpp>
#include <ano/core/range/view/zip.hpp>

#include <concepts/concepts.hpp>
#include <range/v3/algorithm/equal.hpp>


namespace ano
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
    static_assert(ano::is_a<LhsImage, ano::details::Image>());
    static_assert(ano::is_a<RhsImage, ano::details::Image>());
    static_assert(::concepts::equality_comparable_with<image_value_t<LhsImage>, image_value_t<RhsImage>>);

    auto&& lhs_vals = lhs.values();
    auto&& rhs_vals = rhs.values();
    auto   r1       = ranges::rows(lhs_vals);
    auto   r2       = ranges::rows(rhs_vals);

    for (auto [lhs_r, rhs_r] : ranges::view::zip(r1, r2))
      // FIXME: with std::equal you gain performances over ranges::equal here
      if (!::ranges::equal(lhs_r, rhs_r))
        return false;

    return true;
  }

} // namespace ano
