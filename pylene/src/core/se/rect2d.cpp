#include <mln/core/se/rect2d.hpp>


namespace mln::experimental::se
{

  rect2d::rect2d(int width, int height)
  {
    mln_precondition(width >= 0 && "A negative width was given.");
    mln_precondition(height >= 0 && "A negative height was given.");

    int xoffset    = width / 2;
    int yoffset    = height / 2;
    m_dpoints.pmin = {static_cast<short>(-yoffset), static_cast<short>(-xoffset)};
    m_dpoints.pmax = {static_cast<short>(yoffset + 1), static_cast<short>(xoffset + 1)};
  }


  rect2d rect2d::dec() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to use the rectangle incrementally.");

    rect2d tmp;
    tmp.m_dpoints.pmin[0] = m_dpoints.pmin[0];
    tmp.m_dpoints.pmax[0] = m_dpoints.pmax[0];
    tmp.m_dpoints.pmin[1] = m_dpoints.pmin[1] - 1;
    tmp.m_dpoints.pmax[1] = m_dpoints.pmin[1];
    return tmp;
  }

  rect2d rect2d::inc() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to use the rectangle incrementally.");

    rect2d tmp;
    tmp.m_dpoints.pmin[0] = m_dpoints.pmin[0];
    tmp.m_dpoints.pmax[0] = m_dpoints.pmax[0];
    tmp.m_dpoints.pmin[1] = m_dpoints.pmax[1] - 1;
    tmp.m_dpoints.pmax[1] = m_dpoints.pmax[1];
    return tmp;
  }

  std::vector<periodic_line2d> rect2d::decompose() const
  {
    if (!is_decomposable())
      throw std::logic_error("Attempting to decompose the rectangle which is not decomposable.");

    std::vector<periodic_line2d> ses;
    const int       v = m_dpoints.pmax[0] - 1;
    const int       h = m_dpoints.pmax[1] - 1;

    if (h > 0)
      ses.emplace_back(point2d{0, 1}, h);
    if (v > 0)
      ses.emplace_back(point2d{1, 0}, v);

    return ses;
  }

  std::vector<periodic_line2d> rect2d::separate() const
  {
    return decompose();
  }


  bool rect2d::is_decomposable() const
  {
    return !m_dpoints.empty();
  }


  bool rect2d::is_incremental() const
  {
    return m_dpoints.pmax[1] > m_dpoints.pmin[1];
  }

  bool rect2d::is_separable() const
  {
    return !m_dpoints.empty();
  }

  int rect2d::radial_extent() const
  {
    return std::max((m_dpoints.pmax[0] - m_dpoints.pmin[0]) / 2,
                    (m_dpoints.pmax[1] - m_dpoints.pmin[1]) / 2);
  }


} // namespace mln::experimental::se
