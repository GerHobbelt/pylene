#pragma once

#include <mln/core/se/private/se_facade.hpp>

#include <mln/core/box.hpp>
#include <mln/core/range/view/ravel.hpp>
#include <mln/core/se/periodic_line2d.hpp>

#include <vector>
/// \file

namespace mln::se
{

  /// \brief Define a dynamic rectangular window anchored at (0,0).
  /// Its width and height are always odd numbers to ensure symmetry.
  struct rect2d : public se_facade<rect2d>
  {
  public:
    using category     = dynamic_neighborhood_tag;
    using incremental  = std::true_type;
    using decomposable = std::true_type;
    using separable    = std::true_type;

    /// Construct an empty rectangle
    rect2d() = default;

    /// Construct a rectangle of size (Width × Height).
    ///
    /// \param width The width of the rectangle. If \p width is even, it is
    /// rounded to the closest lower odd int.
    /// \param height The height of the rectangle. If \p height is even, it is
    /// rounded to the closest lower odd int.
    rect2d(int width, int height);

    /// \brief A WNeighborhood to be added when used incrementally
    rect2d inc() const;

    /// \brief A WNeighborhood to be substracted when used incrementally
    rect2d dec() const;

    /// \brief Return a range of SE offsets
    auto offsets() const { return mln::ranges::view::ravel(m_dpoints); }

    /// \brief Return true if decomposable (for any non-empty rectangle)
    bool is_decomposable() const;

    /// \brief Return true if separable (for any non-empty rectangle)
    bool is_separable() const;

    /// \brief Return true if incremental (if the width is larger than 1)
    bool is_incremental() const;

    /// \brief Return an horizontal line of length \p Width and a vertical
    /// line of length \p Height corresponding to the SE decomposition.
    std::vector<periodic_line2d> decompose() const;

    /// \brief Return an horizontal line of length \p Width and a vertical
    /// line of length \p Height.
    std::vector<periodic_line2d> separate() const;

    /// \brief Return the extent radius
    int radial_extent() const;

    /// \brief Compute the input region of a ROI
    mln::box2d compute_input_region(mln::box2d roi) const;

    /// \brief Compute the output region of a ROI
    mln::box2d compute_output_region(mln::box2d roi) const;

  private:
    mln::box2d m_dpoints;
  };


  struct rect2d_non_decomp : se_facade<rect2d_non_decomp>
  {
  public:
    using category     = dynamic_neighborhood_tag;
    using incremental  = std::true_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    /// Construct an empty rectangle
    rect2d_non_decomp() = default;

    rect2d_non_decomp(const rect2d&);

    /// Construct a rectangle of size (Width × Height).
    ///
    /// \param width The width of the rectangle. If \p width is even, it is
    /// rounded to the closest lower odd int.
    /// \param height The height of the rectangle. If \p height is even, it is
    /// rounded to the closest lower odd int.
    rect2d_non_decomp(int width, int height);

    /// \brief A WNeighborhood to be added when used incrementally
    rect2d_non_decomp inc() const;

    /// \brief A WNeighborhood to be substracted when used incrementally
    rect2d_non_decomp dec() const;

    /// \brief Return a range of SE offsets
    auto offsets() const { return m_rect.offsets(); }

    /// \brief Return true if incremental (if the width is larger than 1)
    bool is_incremental() const;

    /// \brief Return the extent radius
    int radial_extent() const;

    /// \brief Compute the input region of a ROI
    mln::box2d compute_input_region(mln::box2d roi) const;

    /// \brief Compute the output region of a ROI
    mln::box2d compute_output_region(mln::box2d roi) const;

  private:
    rect2d m_rect;
  };

} // namespace mln::se
