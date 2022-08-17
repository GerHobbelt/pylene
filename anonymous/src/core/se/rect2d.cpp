#include <ano/core/se/rect2d.hpp>


namespace ano::se
{

  rect2d::rect2d(int width, int height)
  {
    ano_precondition(width >= 0 && "A negative width was given.");
    ano_precondition(height >= 0 && "A negative height was given.");

    int xoffset = width / 2;
    int yoffset = height / 2;
    m_dpoints   = ano::box2d({-xoffset, -yoffset}, {xoffset + 1, yoffset + 1});
  }


  rect2d rect2d::dec() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to use the rectangle incrementally.");

    const int x1 = m_dpoints.tl(0);
    const int y1 = m_dpoints.tl(1);
    const int y2 = m_dpoints.br(1);

    rect2d tmp;
    tmp.m_dpoints = ano::box2d({x1 - 1, y1}, {x1, y2});
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
    tmp.m_dpoints = ano::box2d({x2 - 1, y1}, {x2, y2});
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
      ses.emplace_back(ano::point2d{1, 0}, h);
    if (v > 0)
      ses.emplace_back(ano::point2d{0, 1}, v);

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

  ano::box2d rect2d::compute_input_region(ano::box2d roi) const
  {
    roi.tl().x() += m_dpoints.tl().x();
    roi.tl().y() += m_dpoints.tl().y();
    roi.br().x() += m_dpoints.br().x() - 1;
    roi.br().y() += m_dpoints.br().y() - 1;
    return roi;
  }

  ano::box2d rect2d::compute_output_region(ano::box2d roi) const
  {
    roi.tl().x() -= m_dpoints.tl().x();
    roi.tl().y() -= m_dpoints.tl().y();
    roi.br().x() -= (m_dpoints.br().x() - 1);
    roi.br().y() -= (m_dpoints.br().y() - 1);
    return roi;
  }


  rect2d_non_decomp::rect2d_non_decomp(const rect2d& r)
    : m_rect{r}
  {
  }

  rect2d_non_decomp::rect2d_non_decomp(int width, int height)
    : m_rect{width, height}
  {
  }

  rect2d_non_decomp rect2d_non_decomp::dec() const
  {
    return m_rect.dec();
  }

  rect2d_non_decomp rect2d_non_decomp::inc() const
  {
    return m_rect.inc();
  }

  bool rect2d_non_decomp::is_incremental() const
  {
    return m_rect.is_incremental();
  }

  int rect2d_non_decomp::radial_extent() const
  {
    return m_rect.radial_extent();
  }

  ano::box2d rect2d_non_decomp::compute_input_region(ano::box2d roi) const
  {
    return m_rect.compute_input_region(roi);
  }

  ano::box2d rect2d_non_decomp::compute_output_region(ano::box2d roi) const
  {
    return m_rect.compute_output_region(roi);
  }

} // namespace ano::se
