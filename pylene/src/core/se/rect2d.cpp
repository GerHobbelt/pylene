#include <mln/core/se/rect2d.hpp>


namespace mln::se
{

  rect2d::rect2d(int width, int height)
  {
    mln_precondition(width >= 0 && "A negative width was given.");
    mln_precondition(height >= 0 && "A negative height was given.");

    int xoffset    = width / 2;
    int yoffset    = height / 2;
    m_dpoints      = mln::box2d({-xoffset, -yoffset}, {xoffset + 1, yoffset + 1});
  }


  rect2d rect2d::dec() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to use the rectangle incrementally.");

    const int x1 = m_dpoints.tl(0);
    const int y1 = m_dpoints.tl(1);
    const int y2 = m_dpoints.br(1);

    rect2d tmp;
    tmp.m_dpoints = mln::box2d({x1 - 1, y1}, {x1, y2});
    return tmp;
  }

  rect2d rect2d::inc() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to use the rectangle incrementally.");

    const int y1 = m_dpoints.tl(1);
    const int x2 = m_dpoints.br(0);
    const int y2 = m_dpoints.br(1);

    rect2d tmp;
    tmp.m_dpoints = mln::box2d({x2 - 1, y1}, {x2, y2});
    return tmp;
  }

  std::vector<periodic_line2d> rect2d::decompose() const
  {
    if (!is_decomposable())
      throw std::logic_error("Attempting to decompose the rectangle which is not decomposable.");

    std::vector<periodic_line2d> ses;

    const int x0 = m_dpoints.br(0);
    const int y0 = m_dpoints.br(1);
    const int v  = y0 - 1;
    const int h  = x0 - 1;

    if (h > 0)
      ses.emplace_back(mln::point2d{1, 0}, h);
    if (v > 0)
      ses.emplace_back(mln::point2d{0, 1}, v);

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
    int x0 = m_dpoints.tl(0);
    int x1 = m_dpoints.br(0);
    return x1 > x0;
  }

  bool rect2d::is_separable() const
  {
    return !m_dpoints.empty();
  }

  int rect2d::radial_extent() const
  {
    const int x0 = m_dpoints.tl(0);
    const int x1 = m_dpoints.br(0);
    const int y0 = m_dpoints.tl(1);
    const int y1 = m_dpoints.br(1);

    return std::max(x1 - x0, y1 - y0) / 2;
  }

  mln::box2d rect2d::compute_input_region(mln::box2d roi) const
  {
    roi.tl().x() += m_dpoints.tl().x();
    roi.tl().y() += m_dpoints.tl().y();
    roi.br().x() += m_dpoints.br().x() - 1;
    roi.br().y() += m_dpoints.br().y() - 1;
    return roi;
  }

  mln::box2d rect2d::compute_output_region(mln::box2d roi) const
  {
    roi.tl().x() -= m_dpoints.tl().x();
    roi.tl().y() -= m_dpoints.tl().y();
    roi.br().x() -= (m_dpoints.br().x() - 1);
    roi.br().y() -= (m_dpoints.br().y() - 1);
    return roi;
  }

} // namespace mln::se
