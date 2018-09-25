#pragma once
# include <mln/core/neighborhood/neighborhood_base.hpp>
# include <mln/core/se/periodic_line2d.hpp>
# include <vector>

/// \file

namespace mln
{
  namespace se
  {


    struct sphere : dyn_neighborhood_base<dynamic_neighborhood_tag, sphere>
    {
      using is_incremental = std::true_type;
      using is_decomposable = std::false_type;
      using dec_type = dyn_neighborhood<std::vector<point3d>, dynamic_neighborhood_tag>;
      using inc_type = dyn_neighborhood<std::vector<point3d>, dynamic_neighborhood_tag>;


      /// Constructor
      ///
      /// \param radius The radius r of the sphere.
      /// \param approximation Must be 0 or 8 (default: 8)
      sphere(float radius);

      /// \brief Return a range of SE offsets
      std::vector<point3d> offsets() const;

      /// \brief True if the SE is decomposable (always false for now)
      constexpr bool decomposable() const { return false; }

      /// \brief A WNeighborhood to be added when used incrementally
      inc_type inc() const;

      /// \brief A WNeighborhood to be substracted when used incrementally
      dec_type dec() const;

      /// \brief Return the radius of the sphere.
      float   radius() const { return m_radius; }

    private:
      float m_radius;
    };


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    inline
    sphere::sphere(float radius)
      : m_radius(radius)
    {
      assert(m_radius >= 0);
    }

    inline
    std::vector<point3d> sphere::offsets() const
    {
      typedef point3d::value_type P;

      int r = static_cast<int>(m_radius);
      int extent = 2 * r + 1;
      float radius_sqr = m_radius * m_radius;

      std::vector<point3d> dpoints;
      dpoints.reserve(extent * extent * extent);

      for (int i = -r; i <= r; ++i)
        for (int j = -r; j <= r; ++j)
          for (int k = -r; k <= r; ++k)
            if (i*i + j*j + k*k <= radius_sqr)
            {
              point3d p = {(P)i, (P)j, (P)k};
              dpoints.push_back(p);
            }
      return dpoints;
    }

    inline
    sphere::dec_type sphere::dec() const
    {
      typedef point3d::value_type P;
      const int r = static_cast<int>(m_radius);
      const int extent = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point3d> vdec;
      vdec.reserve(extent * extent);
      for (int z = -r; z <= r; ++z)
        for (int y = -r; y <= r; ++y)
          for (int x = -r; x <= 0; ++x)
            if (z*z + y*y + x*x <= radius_sqr)
            {
              point3d p = {(P) z, (P)y, (P)(x - 1)}; // before begin of the line
              vdec.push_back(p);
              break;
            }

      return dec_type(std::move(vdec));
    }

    inline
    sphere::inc_type sphere::inc() const
    {
      typedef point3d::value_type P;
      const int r = static_cast<int>(m_radius);
      const int extent = 2 * r + 1;
      const float radius_sqr = m_radius * m_radius;

      std::vector<point3d> vinc;
      vinc.reserve(extent * extent);
      for (int z = -r; z <= r; ++z)
        for (int y = -r; y <= r; ++y)
          for (int x = r; x >= 0; --x)
            if (z*z + y*y + x*x <= radius_sqr)
            {
              point3d p = {(P) z, (P)y, (P)(x)}; // before begin of the line
              vinc.push_back(p);
              break;
            }

      return inc_type(std::move(vinc));
    }

  } // end of namespace mln::se
} // end of namespace mln

