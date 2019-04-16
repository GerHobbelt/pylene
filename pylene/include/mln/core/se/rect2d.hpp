#pragma once

#include <mln/core/domain/box.hpp>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/se/periodic_line2d.hpp>
#include <mln/core/se/private/se_facade.hpp>

#include <vector>
/// \file

namespace mln
{
  namespace se
  {
    struct rect2d;
  }


  namespace experimental::se
  {

    /// \brief Define a dynamic rectangular window anchored at (0,0).
    /// Its width and height are always odd numbers to ensure symmetry.
    class rect2d : public se_facade<rect2d>
    {
    private:
      using category = dynamic_neighborhood_tag;
      using incremental = std::true_type;
      using decomposable = std::true_type;
      using separable = std::true_type;

    public:
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
      const mln::box2d& offsets() const { return m_dpoints; }

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

    private:
      box2d m_dpoints;
    };


  }

  /**************************/
  /***  Implementation     **/
  /**************************/

  namespace se
  {

    /// \brief Define a dynamic rectangular window anchored at (0,0).
    /// Its width and height are always odd numbers to ensure symmetry.
    struct rect2d
#ifndef MLN_DOXYGEN
      : dyn_neighborhood_base<dynamic_neighborhood_tag, rect2d>
#endif
    {
      typedef std::true_type is_incremental;
      typedef std::true_type is_decomposable;

      typedef dyn_neighborhood<box2d, dynamic_neighborhood_tag> dec_type;
      typedef dyn_neighborhood<box2d, dynamic_neighborhood_tag> inc_type;

      /// Construct an empty rectangle
      rect2d() = default;

      /// Construct a rectangle of size (Width × Height).
      ///
      /// \param width The width of the rectangle. If \p width is even, it is
      /// rounded to the closest lower odd int.
      /// \param height The height of the rectangle. If \p height is even, it is
      /// rounded to the closest lower odd int.
      rect2d(int width, int height)
      {
        mln_precondition(width >= 0 && "A negative width was given.");
        mln_precondition(height >= 0 && "A negative height was given.");

        int xoffset    = width / 2;
        int yoffset    = height / 2;
        m_dpoints.pmin = {static_cast<short>(-yoffset), static_cast<short>(-xoffset)};
        m_dpoints.pmax = {static_cast<short>(yoffset + 1), static_cast<short>(xoffset + 1)};
      }

      /// \brief A WNeighborhood to be added when used incrementally
      inc_type inc() const
      {
        box2d b   = this->m_dpoints;
        b.pmin[1] = b.pmax[1] - 1;
        return b;
      }

      /// \brief A WNeighborhood to be substracted when used incrementally
      dec_type dec() const
      {
        box2d b = this->m_dpoints;
        b.pmin[1] -= 1;
        b.pmax[1] = b.pmin[1] + 1;
        return b;
      }

      /// \brief Return a range of SE offsets
      const mln::box2d& offsets() const { return m_dpoints; }

      /// \brief Return true for any non-empty rectangle
      bool decomposable() const { return !m_dpoints.empty(); }


      int radial_extent() const
      {
        return std::max(m_dpoints.pmax[1] - m_dpoints.pmin[1], m_dpoints.pmax[0] - m_dpoints.pmin[0]) / 2;
      }

      /// \brief Return an horizontal line of length \p Width and a vertical
      /// line of length \p Height corresponding to the SE decomposition.
      std::array<periodic_line2d, 2> decompose() const
      {
        const int       v = m_dpoints.pmax[0] - 1;
        const int       h = m_dpoints.pmax[1] - 1;
        periodic_line2d hline(point2d{0, 1}, h);
        periodic_line2d vline(point2d{1, 0}, v);

        return {{hline, vline}};
      }

    private:
      box2d m_dpoints;
    };

  } // namespace se
} // namespace mln
