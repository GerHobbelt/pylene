#pragma once


#include <mln/core/box.hpp>
#include <mln/core/se/custom_se.hpp>
#include <mln/core/se/periodic_line2d.hpp>
#include <mln/core/se/private/se_facade.hpp>

#include <mln/core/neighborhood/neighborhood_base.hpp>


#include <range/v3/view/span.hpp>
#include <vector>


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
      using is_incremental  = std::true_type;
      using is_decomposable = std::true_type;
      using dec_type        = dyn_neighborhood<std::vector<point2d>, dynamic_neighborhood_tag>;
      using inc_type        = dyn_neighborhood<std::vector<point2d>, dynamic_neighborhood_tag>;


      /// Constructor
      ///
      /// \param radius The radius r of the disc.
      /// \param approximation Must be 0 or 8 (default: 8)
      explicit disc(float radius, int approximation = 8);

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
      float radius() const { return m_radius; }

      /// \brief Return the radial extent of the disc.
      int radial_extent() const { return static_cast<int>(m_radius); }


    private:
      float m_radius;
      int   m_nlines; // number of periodic lines for decomposition (0 for the euclidean disc)
    };

    namespace details
    {
      // Compute the best coefficients for 8-lines decomposition
      std::array<int, 3> disc_compute_decomposition_coeff(int radius);

    } // namespace details

  } // namespace se

  namespace experimental::se
  {

    /// Create a disc of a given radius r with or without approximation.
    ///
    ///
    /// The extent of the structuring will be 2*⌊r⌋+1. If an n-approximation is
    /// given, the radial decomposition of the disc is used when possible.  The
    /// approximiation is a 2-n side polygon. If 0, the exact euclidean disc is
    /// used, that are all points \f$ \{p \mid |p| \le r\} \f$
    ///
    /// \rst
    ///
    /// ============== ======================================
    ///  Property
    /// ============== ======================================
    ///  Incremental   Yes if created with no-approximation
    ///  Decomposable  Yes if created with approximation
    ///  Separable     No
    /// ============== ======================================
    ///
    /// \endrst
    class disc : public se_facade<disc>
    {
      using inc_type = custom_se<::ranges::span<point2d>>;

    public:
      /// Enumeration of disc approximation
      enum approx
      {
        EXACT            = 0, ///< No approximation
        PERIODIC_LINES_8 = 8  ///< Approximation with 8 periodic lines
      };


      using category     = dynamic_neighborhood_tag;
      using incremental  = std::true_type;
      using decomposable = std::true_type;
      using separable    = std::false_type;

      /// Constructs a disc of radius \p r with a given approximation.
      ///
      /// \param radius The radius r of the disc.
      /// \param approximation The disc approximation
      explicit disc(float radius, approx approximation = PERIODIC_LINES_8);

      /// \brief A WNeighborhood to be added when used incrementally
      /// \exception std::runtime_error if the disc is not incremental
      inc_type inc() const;

      /// \brief A WNeighborhood to be substracted when used incrementally
      /// \exception std::runtime_error if this disc is not incremental
      inc_type dec() const;

      /// \brief Return a range of SE for decomposition
      /// \exception std::runtime_error if not decomposable
      std::vector<mln::experimental::se::periodic_line2d> decompose() const;

      /// \brief Return a range of SE offsets
      ::ranges::span<point2d> offsets() const;

      /// \brief Return a range of SE offsets before center
      ::ranges::span<point2d> before_offsets() const;

      /// \brief Return a range of SE offsets after center
      ::ranges::span<point2d> after_offsets() const;


      /// \brief True if the SE is decomposable (i.e. constructed with approximation)
      bool is_decomposable() const;

      /// \brief True if the SE is incremental (i.e. constructed with no-approximation)
      bool is_incremental() const;

      /// \brief Returns the radius of the disc.
      float radius() const { return m_radius; }

      /// \brief Returns the extent radius
      int radial_extent() const { return static_cast<int>(m_radius); }

      /// \brief Return the input ROI for 2D box.
      mln::experimental::box2d compute_input_region(mln::experimental::box2d roi) const;

      /// \brief Return the output ROI for 2D box.
      mln::experimental::box2d compute_output_region(mln::experimental::box2d roi) const;


    private:
      struct cache_data_t
      {
        std::vector<mln::experimental::point2d> m_points;  // Disc points + inc points + dec points
        std::ptrdiff_t                          m_se_size; // Number of points in the no-approx disc
      };


      std::shared_ptr<cache_data_t> _get_data() const;
      std::shared_ptr<cache_data_t> __compute_data() const;

    private:
      mutable std::shared_ptr<cache_data_t> m_data = nullptr; // Cache
      float                                 m_radius;
      int m_nlines; // number of periodic lines for decomposition (0 for the euclidean disc)
    };

  } // namespace experimental::se

} // namespace mln
