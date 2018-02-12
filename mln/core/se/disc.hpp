#ifndef MLN_CORE_SE_DISC_HPP
# define MLN_CORE_SE_DISC_HPP

# include <mln/core/neighborhood/neighborhood_base.hpp>
# include <mln/core/se/periodic_line2d.hpp>
# include <vector>

/// \file

namespace mln
{
  namespace se
  {

    /// Create a disc of a given radius r with or without approximation.
    ///
    ///
    /// The extent of the structuring will be 2*⌊r⌋+1. If an n-approximation is
    /// given, the radial decomposition of the disc is used when possible.  The
    /// approximiation is a 2-n side polygon. If 0, the exact euclidean disc is
    /// used, that are all points \f$ \{p \mid |p| \le r\} \f$
    struct disc
#ifndef MLN_DOXYGEN
      : dyn_neighborhood_base<dynamic_neighborhood_tag, disc>
#endif
    {
      using is_incremental = std::true_type;
      using is_decomposable = std::true_type;
      using dec_type = dyn_neighborhood<std::vector<point2d>, dynamic_neighborhood_tag>;
      using inc_type = dyn_neighborhood<std::vector<point2d>, dynamic_neighborhood_tag>;


      /// Constructor
      ///
      /// \param radius The radius r of the disc.
      /// \param approximation Must be 0 or 8 (default: 8)
      disc(float radius, int approximation = 8);

      /// \brief Return a range of SE for decomposition
      std::vector<periodic_line2d> decompose() const;

      /// \brief Return a range of SE offsets
      std::vector<point2d> offsets() const;

      /// \brief True if the SE is decomposable (i.e. constructed with no-approximation)
      bool decomposable() const;

      /// \brief A WNeighborhood to be added when used incrementally
      inc_type inc() const;

      /// \brief A WNeighborhood to be substracted when used incrementally
      dec_type dec() const;

      /// \brief Return the radius of the disc.
      float   radius() const { return m_radius; }

      // protected to allow testing
      static std::array<int, 3> _compute_decomposition_coeff(int radius, int nlines);

    private:
      float m_radius;
      int   m_nlines; // number of periodic lines for decomposition (0 for the euclidean disc)
    };


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    inline
    disc::disc(float radius, int approximation)
      : m_radius(radius),
        m_nlines(approximation)
    {
      assert(m_radius >= 0);
      assert(m_nlines == 0 || m_nlines == 2 || m_nlines == 4 || m_nlines == 8);
    }

    inline
    bool disc::decomposable() const
    {
      return m_nlines > 0;
    }

    inline
    std::array<int, 3> disc::_compute_decomposition_coeff(int radius, int nlines)
    {
      (void) nlines;
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
        const float a0 = 0.22498089194617754;
        const float b0 = -0.74997778133276127;
        const float a2 = 0.092868889904065763;
        const float b2 = 0.03471904979442568;

        k0 = (radius < 11) ? (1 + (radius+1) % 2) : static_cast<int>(std::round(a0 * radius + b0));
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

    inline
    std::vector<periodic_line2d> disc::decompose() const
    {
      mln_precondition(m_nlines > 0);

      std::vector<periodic_line2d> lines;
      lines.reserve(m_nlines);

      std::array<int, 3> k = _compute_decomposition_coeff(static_cast<int>(m_radius), m_nlines);


      const point2d se[] = {{0,1}, {1,0}, {1, 1}, {1, -1}, {1, 2}, {2, 1}, {2, -1}, {1, -2}};
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

    inline
    std::vector<point2d> disc::offsets() const
    {
      typedef point2d::value_type P;

      int r = static_cast<int>(m_radius);
      int extent = 2 * r + 1;
      float radius_sqr = m_radius * m_radius;

      std::vector<point2d> dpoints;
      dpoints.reserve(extent * extent);

      for (int i = -r; i <= r; ++i)
        for (int j = -r; j <= r; ++j)
          if (i*i + j*j <= radius_sqr)
          {
            point2d p = {(P)i, (P)j};
            dpoints.push_back(p);
          }
      return dpoints;
    }

    inline
    disc::dec_type disc::dec() const
    {
      typedef point2d::value_type P;
      const int r = static_cast<int>(m_radius);
      const int extent = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point2d> vdec;
      vdec.reserve(extent);
      for (int y = -r; y <= r; ++y)
      {
        for (int x = -r; x <= 0; ++x)
          if (y*y + x*x <= radius_sqr)
          {
            point2d p = {(P)y, (P)(x - 1)}; // before begin of the line
            vdec.push_back(p);
            break;
          }
      }
      return dec_type(std::move(vdec));
    }

    inline
    disc::inc_type disc::inc() const
    {
      typedef point2d::value_type P;
      const int r = static_cast<int>(m_radius);
      const int extent = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point2d> vinc;
      vinc.reserve(extent);
      for (int y = -r; y <= r; ++y)
      {
        for (int x = r; x >= 0; --x)
          if (y*y + x*x <= radius_sqr)
          {
            point2d p = {(P)y, (P)x}; // last point of the line
            vinc.push_back(p);
            break;
          }
      }

      return inc_type(std::move(vinc));
    }

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

  } // end of namespace mln::se
} // end of namespace mln

#endif //!MLN_CORE_SE_DISC_HPP
