#include <mln/core/se/disc.hpp>
#include <mln/core/assert.hpp>

#include <stdexcept>
#include <array>


namespace mln::se::details
{

  std::array<int, 3> disc_compute_decomposition_coeff(int radius)
  {
    // We decompose a disc of radius r
    // D = k₁ * Lθ₁ + k₂ * Lθ₂ + ... + kₙ * Lθₙ
    // with k₁ = k₂ = kₙ
    
    // Those are coefficient for periodic lines:
    // k0: coeff for 0°, 90° lines
    // k1: coeff for 45°, 135° lines
    // k2: coeff for 30°, 60°, 120°, 150° lines
    int k0 = 0;
    int k1 = 0;
    int k2 = 0;
    if (radius > 0)
    {
      // Empirically, we have found the best k0, k1, k2 values
      // that minimizes the error with the real euclidean disc.
      // The linear approximation of k0(r) and k2(r) have a +- 1 error,
      // they are close enough to use them. So:
      // k0(r) = a0 * t + b0
      // k2(r) = a2 * t + b2
      const float a0 = 0.22498089194617754f;
      const float b0 = -0.74997778133276127f;
      const float a2 = 0.092868889904065763f;
      const float b2 = 0.03471904979442568f;

      k0 = (radius < 11) ? (1 + (radius + 1) % 2) : static_cast<int>(std::round(a0 * radius + b0));
      k2 = (radius < 7) ? 0 : static_cast<int>(std::round(a2 * radius + b2));

      // Compute the missing value k1
      // k0 += 1 => radial extent += 1
      // k1 += 1 => radial extent += 2
      // k2 += 1 => radial extent += 6

      int r = radius - k0 - 6 * k2;
      if (r > 0)
      {
        k1 = r / 2;
        k0 += r % 2;
      }
    }
    return {{k0, k1, k2}};
  }

} // namespace

namespace mln::se
{
  disc::disc(float radius, int approximation)
    : m_radius(radius)
    , m_nlines(approximation)
  {
    assert(m_radius >= 0);
    assert(m_nlines == 0 || m_nlines == 2 || m_nlines == 4 || m_nlines == 8);
  }

  bool disc::decomposable() const { return m_nlines > 0; }


  std::vector<periodic_line2d> disc::decompose() const
  {
    mln_precondition(m_nlines > 0);

    std::vector<periodic_line2d> lines;
    lines.reserve(m_nlines);

    std::array<int, 3> k = details::disc_compute_decomposition_coeff(static_cast<int>(m_radius));


    const point2d se[] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}, {1, 2}, {2, 1}, {2, -1}, {1, -2}};
    lines.push_back(periodic_line2d(se[0], k[0]));
    lines.push_back(periodic_line2d(se[1], k[0]));

    if (k[1] > 0)
    {
      lines.push_back(periodic_line2d(se[2], k[1]));
      lines.push_back(periodic_line2d(se[3], k[1]));
    }

    if (k[2] > 0)
    {
      lines.push_back(periodic_line2d(se[4], k[2]));
      lines.push_back(periodic_line2d(se[5], k[2]));
      lines.push_back(periodic_line2d(se[6], k[2]));
      lines.push_back(periodic_line2d(se[7], k[2]));
    }

    return lines;
    }

    std::vector<point2d> disc::offsets() const
    {
      typedef point2d::value_type P;

      int   r          = static_cast<int>(m_radius);
      int   extent     = 2 * r + 1;
      float radius_sqr = m_radius * m_radius;

      std::vector<point2d> dpoints;
      dpoints.reserve(extent * extent);

      for (int i = -r; i <= r; ++i)
        for (int j = -r; j <= r; ++j)
          if (i * i + j * j <= radius_sqr)
          {
            point2d p = {(P)i, (P)j};
            dpoints.push_back(p);
          }
      return dpoints;
    }

    disc::dec_type disc::dec() const
    {
      using P                = point2d::value_type;

      const int   r          = static_cast<int>(m_radius);
      const int   extent     = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point2d> vdec;
      vdec.reserve(extent);
      for (int y = -r; y <= r; ++y)
      {
        for (int x = -r; x <= 0; ++x)
          if (y * y + x * x <= radius_sqr)
          {
            point2d p = {(P)y, (P)(x - 1)}; // before begin of the line
            vdec.push_back(p);
            break;
          }
      }
      return dec_type(std::move(vdec));
    }

    disc::inc_type disc::inc() const
    {
      using P                = point2d::value_type;
      const int   r          = static_cast<int>(m_radius);
      const int   extent     = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point2d> vinc;
      vinc.reserve(extent);
      for (int y = -r; y <= r; ++y)
      {
        for (int x = r; x >= 0; --x)
          if (y * y + x * x <= radius_sqr)
          {
            point2d p = {(P)y, (P)x}; // last point of the line
            vinc.push_back(p);
            break;
          }
      }

      return inc_type(std::move(vinc));
    }

}


namespace mln::experimental::se
{

  disc::disc(float radius, approx approximation)
    : m_radius(radius)
    , m_nlines(static_cast<int>(approximation))
  {
    mln_precondition(m_radius >= 0);
  }


  bool disc::is_decomposable() const
  {
    return m_nlines > 0;
  }

  bool disc::is_incremental() const
  {
    return m_nlines > 0;
  }


  std::vector<mln::experimental::se::periodic_line2d> disc::decompose() const
  {
    if (!is_decomposable())
      throw std::logic_error("Attempting to decompose the disc which is not decomposable.");

    std::vector<mln::experimental::se::periodic_line2d> lines;
    lines.reserve(m_nlines);

    const point2d se[] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}, {1, 2}, {2, 1}, {2, -1}, {1, -2}};
    std::array<int, 3> k;

    if (m_nlines == 8)
    {
      k = mln::se::details::disc_compute_decomposition_coeff(static_cast<int>(m_radius));
    }
    else
    {
       throw std::runtime_error("Not implemented.");
    }

    lines.push_back(mln::experimental::se::periodic_line2d(se[0], k[0]));
    lines.push_back(mln::experimental::se::periodic_line2d(se[1], k[0]));

    if (k[1] > 0)
    {
      lines.push_back(mln::experimental::se::periodic_line2d(se[2], k[1]));
      lines.push_back(mln::experimental::se::periodic_line2d(se[3], k[1]));
    }

    if (k[2] > 0)
    {
      lines.push_back(mln::experimental::se::periodic_line2d(se[4], k[2]));
      lines.push_back(mln::experimental::se::periodic_line2d(se[5], k[2]));
      lines.push_back(mln::experimental::se::periodic_line2d(se[6], k[2]));
      lines.push_back(mln::experimental::se::periodic_line2d(se[7], k[2]));
    }
    return lines;
  }

  [[gnu::noinline]] void disc::_compute_data() const
  {
    mln_precondition(m_data == nullptr);
    // Note that const method can be called from several threads
    auto                          data     = this->__compute_data();
    std::shared_ptr<cache_data_t> expected = nullptr;
    std::atomic_compare_exchange_strong(&m_data, &expected, data);
  }


  ::ranges::span<point2d> disc::offsets() const
  {
    if (m_data == nullptr)
      _compute_data();
    return {m_data->m_points.data(), m_data->m_se_size};
  }

  ::ranges::span<point2d> disc::before_offsets() const
  {
    if (m_data == nullptr)
      _compute_data();
    return {m_data->m_points.data(), m_data->m_se_size / 2};
  }

  ::ranges::span<point2d> disc::after_offsets() const
  {
    if (m_data == nullptr)
      _compute_data();
    return {m_data->m_points.data() + m_data->m_se_size / 2 + 1, m_data->m_se_size / 2};
  }

  disc::inc_type disc::dec() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to decompose the disc which is not incremental.");

    if (m_data == nullptr)
      _compute_data();

    int r      = static_cast<int>(m_radius);
    int extent = 2 * r + 1;
    ::ranges::span<point2d> points{m_data->m_points.data() + m_data->m_se_size, extent};
    return {points, r};
  }

  disc::inc_type disc::inc() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to decompose the disc which is not incremental.");

    if (m_data == nullptr)
      _compute_data();

    int r      = static_cast<int>(m_radius);
    int extent = 2 * r + 1;
    ::ranges::span<point2d> points{m_data->m_points.data() + m_data->m_se_size + extent, extent};
    return {points, r};
  }



  std::shared_ptr<disc::cache_data_t> disc::__compute_data() const
  {
    typedef point2d::value_type P;

    int   r          = static_cast<int>(m_radius);
    int   extent     = 2 * r + 1;
    float radius_sqr = m_radius * m_radius;

    std::vector<point2d> buffer;
    buffer.reserve(extent * extent + extent + extent);

    // All points
    for (int y = -r; y <= r; ++y)
      for (int x = -r; x <= r; ++x)
        if (y * y + x * x <= radius_sqr)
          buffer.push_back(point2d{(P)y, (P)x});
    std::size_t se_size = buffer.size();

    // Dec Points
    for (int y = -r; y <= r; ++y)
      for (int x = -r; x <= 0; ++x)
        if (y * y + x * x <= radius_sqr)
        {
          buffer.push_back(point2d{(P)y, (P)(x - 1)}); // before begin of the line
          break;
        }

    // Inc Points
    for (int y = -r; y <= r; ++y)
      for (int x = r; x >= 0; --x)
        if (y * y + x * x <= radius_sqr)
        {
          buffer.push_back(point2d{(P)y, (P)x}); // last point of the line
          break;
        }

    auto data = std::make_shared<cache_data_t>();
    data->m_points = std::move(buffer);
    data->m_se_size = se_size;

    mln_assertion(se_size % 2 == 1);

    return data;
  }


} // namespace mln::experimental::se


/*
//This the matlab decomposition, the approximation is worst than ours

    inline
    std::vector<periodic_line2d> disc::decompose() const
    {
      std::vector<periodic_line2d> lines;
      lines.reserve(m_nlines);

      const point2d se[] = {{0,1}, {1,0}, {1, 1}, {1, -1}, {1, 2}, {2, 1}, {2, -1}, {1, -2}};

      // We decompose a disc of radius r
      // D = k₁ * Lθ₁ + k₂ * Lθ₂ + ... + kₙ * Lθₙ
      // with k₁ = k₂ = kₙ
      // D is a 2n polygon with mininimal radius
      // rmin = k.tan⁻¹(π/2n)
      // rmax = k.sin⁻¹(π/2n)
      float angle = M_PI / (2 * m_nlines);
      float se_objective_radius = 2 * m_radius / (1.f / std::sin(angle) + 1.f / std::tan(angle));

      int current_radius = 0;
      for (auto dp : se)
      {
        // Compute k, the number of repetition of the se.
        float se_elementary_length = std::hypot(dp[0], dp[1]);
        int k = static_cast<int>(se_objective_radius / se_elementary_length);
        if (k > 0)
        {
          int se_size = 2 * k + 1;
          lines.push_back(periodic_line2d(-dp * k, dp, se_size));
          current_radius += std::abs(dp[0]) * k;
        }
      }

      // The current radius is too small
      // We pad it with pure horizontal and vertical lines
      {
        int k = static_cast<int>(m_radius) - current_radius;
        int se_size = 2 * k + 1;
        lines.push_back(periodic_line2d(point2d{-1,0} * k, point2d{1,0}, se_size));
        lines.push_back(periodic_line2d(point2d{0,-1} * k, point2d{0,1}, se_size));
      }

      return lines;
    }
*/
