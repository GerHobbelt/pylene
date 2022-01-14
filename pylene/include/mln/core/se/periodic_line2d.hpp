#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/box.hpp>

/// \file

namespace mln::se
{

  /// Create a line with points points equally spaced from the origin
  /// in a given direction \p V of length \f$l = 2n+1\f$.
  ///
  /// \f[
  /// L_{n,V} = \{ -n.V, -(n-1).V, ..., -V, (0,0), V, ..., (n-1).V, n.V \}
  /// \f]
  ///
  /// \p V defines the *period* of the line.
  class periodic_line2d
#ifndef MLN_DOXYGEN
    : public neighborhood_facade<periodic_line2d>
#endif
  {

    class rng_t : public ::ranges::view_facade<rng_t>
    {
      friend ::ranges::range_access;
      mln::point2d m_cur;
      mln::point2d m_delta;
      std::size_t                m_k;

      auto read() const { return m_cur; }
      bool equal(::ranges::default_sentinel_t) const { return m_k == 0; }
      bool equal(const rng_t& other) const { return m_k == other.m_k; }
      void next()
      {
        m_cur += m_delta;
        --m_k;
      }

    public:
      rng_t() = default;
      rng_t(mln::point2d start, mln::point2d delta, std::size_t k)
        : m_cur{start}
        , m_delta{delta}
        , m_k{k}
      {
      }
    };

  public:
    using category     = dynamic_neighborhood_tag;
    using separable    = std::false_type;
    using incremental  = std::false_type;
    using decomposable = std::false_type;


    /// \brief Create a line of period \p V and number of pixels \f$L = 2k+1\f$
    ///
    /// \param V The period.
    /// \param k Half-number of pixels in the line.
    /// \precondition k >= 0
    periodic_line2d(mln::point2d V, int k) noexcept;

    /// \brief Return a range of SE offsets
    rng_t offsets() const noexcept;
    rng_t before_offsets() const noexcept;
    rng_t after_offsets() const noexcept;

    /// \brief Return the number of pixels in the line
    int size() const noexcept { return 2 * m_k + 1; }

    /// \brief Return the number of repetitions \p k
    int repetition() const noexcept { return m_k; }

    /// \brief Return the period
    mln::point2d period() const noexcept { return m_delta; }

    /// \brief Return the extent radius
    int radial_extent() const noexcept;

    /// \brief Return the input region (the outer region needed for the \p roi computation)
    ///
    /// \post ``this->compute_input_region(roi).includes(roi)``
    mln::box2d compute_input_region(mln::box2d roi) const noexcept;

    /// \brief Return the output region (the valid inner region)
    ///
    /// \pre ``roi.includes(this->se.compute_output_region(roi)``
    mln::box2d compute_output_region(mln::box2d roi) const noexcept;

    /// \brief Return true if the line is a pure horizontal (dy=0 and dx=1)
    bool is_horizontal() const noexcept;

    /// \brief Return true if the line is a pure vertical (dx=0 and dy=1)
    bool is_vertical() const noexcept;


  private:
    mln::point2d m_delta;
    int          m_k;
  };

} // namespace mln::se
