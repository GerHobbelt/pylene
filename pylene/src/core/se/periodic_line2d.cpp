#include <mln/core/se/periodic_line2d.hpp>


namespace mln::se
{

  periodic_line2d::periodic_line2d(point2d delta, int k)
  {
    mln_precondition((k >= 0) && "The extent must be positive");

    m_delta = (delta < mln::point2d{0, 0}) ? point2d(-delta) : delta;
    m_k     = k;
  }

  periodic_line2d::iterator periodic_line2d::offsets() const { return iterator(m_delta, m_k); }
} // namespace mln::se


namespace mln::experimental::se
{

  periodic_line2d::periodic_line2d(experimental::point2d delta, int k)
  {
    mln_precondition((k >= 0) && "The extent must be positive");

    m_delta = (delta < mln::experimental::point2d{0, 0}) ? experimental::point2d(-delta) : delta;
    m_k     = k;
  }

  periodic_line2d::rng_t periodic_line2d::offsets() const
  {
    return { -m_k * m_delta, m_delta, static_cast<std::size_t>(2 * m_k + 1) };
  }

  periodic_line2d::rng_t periodic_line2d::before_offsets() const
  {
    return { -m_k * m_delta, m_delta, static_cast<std::size_t>(m_k) };
  }

  periodic_line2d::rng_t periodic_line2d::after_offsets() const
  {
    return { m_delta, m_delta, static_cast<std::size_t>(m_k) };
  }

  int periodic_line2d::radial_extent() const
  {
    return m_k * std::max(std::abs(m_delta.x()), std::abs(m_delta.y()));
  }

  bool periodic_line2d::is_horizontal() const noexcept { return m_delta.y() == 0; }

  bool periodic_line2d::is_vertical() const noexcept { return m_delta.x() == 0; }


  mln::experimental::box2d periodic_line2d::compute_input_region(mln::experimental::box2d roi) const
  {
    int dx = std::abs(m_delta.x()) * m_k;
    int dy = std::abs(m_delta.y()) * m_k;
    roi.tl().x() -= dx;
    roi.br().x() += dx;
    roi.tl().y() -= dy;
    roi.br().y() += dy;
    return roi;
  }

  mln::experimental::box2d periodic_line2d::compute_output_region(mln::experimental::box2d roi) const
  {
    int dx = std::abs(m_delta.x()) * m_k;
    int dy = std::abs(m_delta.y()) * m_k;
    roi.tl().x() += dx;
    roi.br().x() -= dx;
    roi.tl().y() += dy;
    roi.br().y() -= dy;

    if (roi.width() <= 0 || roi.height() <= 0)
      roi = mln::experimental::box2d{};

    return roi;
  }



} // namespace mln::experimental::se
