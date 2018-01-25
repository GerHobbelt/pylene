#ifndef MLN_CORE_SE_RECT2D_HPP
#define MLN_CORE_SE_RECT2D_HPP

#include <mln/core/domain/box.hpp>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/se/periodic_line2d.hpp>

namespace mln
{
  namespace se
  {

    ///
    /// \brief Define a dynamic rectangular window
    ///
    struct rect2d;

    /**************************/
    /***  Implementation     **/
    /**************************/

    struct rect2d : dyn_neighborhood_base<dynamic_neighborhood_tag, rect2d>
    {
      typedef std::true_type is_incremental;
      typedef std::true_type is_decomposable;

      typedef dyn_neighborhood<box2d, dynamic_neighborhood_tag> dec_type;
      typedef dyn_neighborhood<box2d, dynamic_neighborhood_tag> inc_type;

      rect2d() = default;

      rect2d(int width, int height)
      {
        mln_precondition(width >= 0 && "A negative width was given.");
        mln_precondition(height >= 0 && "A negative height was given.");

        int xoffset = width / 2;
        int yoffset = height / 2;
        m_dpoints.pmin = {static_cast<short>(-yoffset), static_cast<short>(-xoffset)};
        m_dpoints.pmax = {static_cast<short>(yoffset+1), static_cast<short>(xoffset+1)};
      }

      inc_type inc() const
      {
        box2d b = this->m_dpoints;
        b.pmin[1] = b.pmax[1] - 1;
        return b;
      }

      dec_type dec() const
      {
        box2d b = this->m_dpoints;
        b.pmin[1] -= 1;
        b.pmax[1] = b.pmin[1] + 1;
        return b;
      }

      const mln::box2d& offsets() const { return m_dpoints; }

      bool decomposable() const { return !m_dpoints.empty(); }

      std::array<periodic_line2d, 2> decompose() const
      {
        const int v = m_dpoints.pmax[0] - 1;
        const int h = m_dpoints.pmax[1] - 1;
        periodic_line2d hline(point2d{0,1}, h);
        periodic_line2d vline(point2d{1,0}, v);

        return {{hline, vline}};
      }

    private:
      box2d m_dpoints;
    };

  } // end of namespace mln::se
} // end of namespace mln

#endif //! MLN_CORE_SE_RECT2D_HPP
