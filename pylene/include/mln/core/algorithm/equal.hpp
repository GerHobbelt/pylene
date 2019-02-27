#ifndef MLN_CORE_ALGORITHM_EQUAL_HPP
#define MLN_CORE_ALGORITHM_EQUAL_HPP

#include <mln/core/image/image.hpp>

#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <range/v3/algorithm/equal.hpp>

namespace mln
{

  /// \brief Compare if two image are equals.
  ///
  /// Two image are said equal if their domains
  /// are equals and have the same values.
  ///
  /// \param ima1 First image
  /// \param ima2 Second image
  ///
  /// \tparam I must model a Readable Forward Image
  /// \tparam J must model a Readable Forward Image
  ///
  /// \return True if image are equals.
  ///
  template <typename I, typename J>
  bool equal(const Image<I>& ima1, const Image<J>& ima2);

  namespace experimental
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
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename I, typename J>
  inline bool equal(const Image<I>& ima1, const Image<J>& ima2)
  {
    mln_pixter(px1, exact(ima1));
    mln_pixter(px2, exact(ima2));

    mln_forall (px1, px2)
      if (px1->point() != px2->point() or px1->val() != px2->val())
        return false;

    return true;
  };

  namespace experimental
  {
    template <class LhsImage, class RhsImage>
    bool equal(LhsImage lhs, RhsImage rhs)
    {
      static_assert(mln::is_a<LhsImage, Image>());
      static_assert(mln::is_a<RhsImage, Image>());

      auto&& lhs_vals = lhs.new_values();
      auto&& rhs_vals = rhs.new_values();

      for (auto [lhs_r, rhs_r] : ranges::view::zip(ranges::rows(lhs_vals), ranges::rows(rhs_vals)))
        if (!::ranges::equal(lhs_r, rhs_r))
          return false;

      return true;
    }
  } // namespace experimental

} // end of namespace mln

#endif //! MLN_CORE_ALGORITHM_EQUAL_HPP
