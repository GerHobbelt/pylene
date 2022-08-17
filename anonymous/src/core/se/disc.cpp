#include <ano/core/assert.hpp>
#include <ano/core/se/disc.hpp>

#include <array>
#include <cmath>
#include <stdexcept>


namespace ano::se::details
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

} // namespace ano::se::details


namespace ano::se
{

  disc::disc(float radius, approx approximation)
    : m_radius(radius)
    , m_nlines(static_cast<int>(approximation))
  {
    ano_precondition(m_radius >= 0);
  }


  bool disc::is_decomposable() const
  {
    return m_nlines > 0;
  }

  bool disc::is_incremental() const
  {
    return m_nlines == 0;
  }


  std::vector<ano::se::periodic_line2d> disc::decompose() const
  {
    if (!is_decomposable())
      throw std::logic_error("Attempting to decompose the disc which is not decomposable.");

    std::vector<ano::se::periodic_line2d> lines;
    lines.reserve(m_nlines);

    const point2d      se[] = {{1, 0}, {0, 1}, {1, 1}, {-1, 1}, {1, 2}, {2, 1}, {-1, 2}, {-2, 1}};
    std::array<int, 3> k;

    if (m_nlines == 8)
    {
      k = ano::se::details::disc_compute_decomposition_coeff(static_cast<int>(m_radius));
    }
    else
    {
      throw std::runtime_error("Not implemented.");
    }

    lines.push_back(ano::se::periodic_line2d(se[0], k[0]));
    lines.push_back(ano::se::periodic_line2d(se[1], k[0]));

    if (k[1] > 0)
    {
      lines.push_back(ano::se::periodic_line2d(se[2], k[1]));
      lines.push_back(ano::se::periodic_line2d(se[3], k[1]));
    }

    if (k[2] > 0)
    {
      lines.push_back(ano::se::periodic_line2d(se[4], k[2]));
      lines.push_back(ano::se::periodic_line2d(se[5], k[2]));
      lines.push_back(ano::se::periodic_line2d(se[6], k[2]));
      lines.push_back(ano::se::periodic_line2d(se[7], k[2]));
    }
    return lines;
  }

  std::shared_ptr<disc::cache_data_t> disc::_get_data() const
  {
    // Note that const method can be called from several threads
    auto data = std::atomic_load(&m_data);
    if (data == nullptr)
    {
      data = this->__compute_data();

      std::shared_ptr<disc::cache_data_t> expected = nullptr;
      if (!std::atomic_compare_exchange_strong(&m_data, &expected, data))
        data = std::move(expected);
    }
    return data;
  }


  ::ranges::span<point2d> disc::offsets() const
  {
    auto data = _get_data();
    return {data->m_points.data(), data->m_se_size};
  }

  ::ranges::span<point2d> disc::before_offsets() const
  {
    auto data = _get_data();
    return {data->m_points.data(), data->m_se_size / 2};
  }

  ::ranges::span<point2d> disc::after_offsets() const
  {
    auto data = _get_data();
    return {data->m_points.data() + data->m_se_size / 2 + 1, data->m_se_size / 2};
  }

  disc::inc_type disc::dec() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to decompose the disc which is not incremental.");

    auto data = _get_data();

    int                     r      = static_cast<int>(m_radius);
    int                     extent = 2 * r + 1;
    ::ranges::span<point2d> points{data->m_points.data() + data->m_se_size, extent};
    return {points, r};
  }

  disc::inc_type disc::inc() const
  {
    if (!is_incremental())
      throw std::logic_error("Attempting to decompose the disc which is not incremental.");

    auto data = _get_data();

    int                     r      = static_cast<int>(m_radius);
    int                     extent = 2 * r + 1;
    ::ranges::span<point2d> points{data->m_points.data() + data->m_se_size + extent, extent};
    return {points, r};
  }


  [[gnu::noinline]] std::shared_ptr<disc::cache_data_t> disc::__compute_data() const
  {
    int   r          = static_cast<int>(m_radius);
    int   extent     = 2 * r + 1;
    float radius_sqr = m_radius * m_radius;

    std::vector<ano::point2d> buffer;
    buffer.reserve(extent * extent + extent + extent);

    // All points
    for (int y = -r; y <= r; ++y)
      for (int x = -r; x <= r; ++x)
        if (y * y + x * x <= radius_sqr)
          buffer.push_back({x, y});
    std::size_t se_size = buffer.size();

    // Dec Points
    for (int y = -r; y <= r; ++y)
      for (int x = -r; x <= 0; ++x)
        if (y * y + x * x <= radius_sqr)
        {
          buffer.push_back({x - 1, y}); // before begin of the line
          break;
        }

    // Inc Points
    for (int y = -r; y <= r; ++y)
      for (int x = r; x >= 0; --x)
        if (y * y + x * x <= radius_sqr)
        {
          buffer.push_back({x, y}); // last point of the line
          break;
        }

    auto data       = std::make_shared<cache_data_t>();
    data->m_points  = std::move(buffer);
    data->m_se_size = se_size;

    ano_assertion(se_size % 2 == 1);

    return data;
  }

  ano::box2d disc::compute_input_region(ano::box2d roi) const
  {
    roi.inflate(radial_extent());
    return roi;
  }


  ano::box2d disc::compute_output_region(ano::box2d roi) const
  {
    roi.inflate(-radial_extent());
    return roi;
  }


  disc_non_decomp::disc_non_decomp(const disc& d)
    : m_disc{d}
  {
  }

  disc_non_decomp::disc_non_decomp(float radius, disc::approx approximation)
    : m_disc{radius, approximation}
  {
  }

  bool disc_non_decomp::is_incremental() const
  {
    return m_disc.is_incremental();
  }


  ::ranges::span<point2d> disc_non_decomp::offsets() const
  {
    return m_disc.offsets();
  }

  ::ranges::span<point2d> disc_non_decomp::before_offsets() const
  {
    return m_disc.before_offsets();
  }

  ::ranges::span<point2d> disc_non_decomp::after_offsets() const
  {
    return m_disc.after_offsets();
  }

  disc::inc_type disc_non_decomp::dec() const
  {
    return m_disc.dec();
  }

  disc::inc_type disc_non_decomp::inc() const
  {
    return m_disc.inc();
  }

  ano::box2d disc_non_decomp::compute_input_region(ano::box2d roi) const
  {
    return m_disc.compute_input_region(roi);
  }

  ano::box2d disc_non_decomp::compute_output_region(ano::box2d roi) const
  {
    return m_disc.compute_output_region(roi);
  }

} // namespace ano::se


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
