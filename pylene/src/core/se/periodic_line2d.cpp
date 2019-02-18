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

  periodic_line2d::periodic_line2d(point2d delta, int k)
  {
    mln_precondition((k >= 0) && "The extent must be positive");

    m_delta = (delta < mln::point2d{0, 0}) ? point2d(-delta) : delta;
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
} // namespace mln::experimental::se
