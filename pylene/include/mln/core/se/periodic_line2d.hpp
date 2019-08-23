#pragma once

#include <mln/core/iterator/iterator_base.hpp>
#include <mln/core/neighborhood/dyn_neighborhood.hpp>
#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>
#include <mln/core/box.hpp>

/// \file

namespace mln
{

  namespace se
  {

    /// Create a line with points points equally spaced from the origin
    /// in a given direction \p V of length \f$l = 2n+1\f$.
    ///
    /// \f[
    /// L_{n,V} = \{ -n.V, -(n-1).V, ..., -V, (0,0), V, ..., (n-1).V, n.V \}
    /// \f]
    ///
    /// \p V defines the *period* of the line.
    struct periodic_line2d
#ifndef MLN_DOXYGEN
      : dyn_neighborhood_base<dynamic_neighborhood_tag, periodic_line2d>
#endif
    {
      struct __iterator;
      using iterator        = __iterator;
      using const_iterator  = __iterator;
      using is_incremental  = std::false_type;
      using is_decomposable = std::false_type;


      /// \brief Create a line of period \p V and number of pixels \f$L = 2k+1\f$
      ///
      /// \param V The period.
      /// \param k Half-number of pixels in the line.
      /// \precondition k >= 0
      periodic_line2d(point2d V, int k);

      /// \brief Return a range of SE offsets
      iterator offsets() const;

      /// \brief Return the number of pixels in the line
      int size() const { return 2 * m_k + 1; }

      /// \brief Return the number of repetitions \p k
      int repetition() const { return m_k; }

      /// \brief Return the period
      point2d period() const { return m_delta; }

      /// \brief Return the radial extent
      int radial_extent() const { return m_k; }

    private:
      point2d m_delta;
      int     m_k;
    };

  } // namespace se

  namespace experimental::se
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
        mln::experimental::point2d m_cur;
        mln::experimental::point2d m_delta;
        std::size_t m_k;

        auto read() const { return m_cur; }
        bool equal(::ranges::default_sentinel) const { return m_k == 0; }
        bool equal(const rng_t& other) const { return m_k == other.m_k; }
        void next() { m_cur += m_delta; --m_k; }
      public:
        rng_t() = default;
        rng_t(mln::experimental::point2d start, mln::experimental::point2d delta, std::size_t k)
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
      periodic_line2d(mln::experimental::point2d V, int k);

      /// \brief Return a range of SE offsets
      rng_t offsets() const;
      rng_t before_offsets() const;
      rng_t after_offsets() const;

      /// \brief Return the number of pixels in the line
      int size() const { return 2 * m_k + 1; }

      /// \brief Return the number of repetitions \p k
      int repetition() const { return m_k; }

      /// \brief Return the period
      mln::experimental::point2d period() const { return m_delta; }

      /// \brief Return the extent radius
      int radial_extent() const;

      /// \brief
      mln::experimental::box2d compute_input_region(mln::experimental::box2d roi) const;

      /// \brief
      mln::experimental::box2d compute_output_region(mln::experimental::box2d roi) const;

    private:
      mln::experimental::point2d m_delta;
      int     m_k;
    };

  } // namespace experimental::se


  /******************************************/
  /****          Implementation          ****/
  /******************************************/
  namespace se
  {

    struct periodic_line2d::__iterator : iterator_base<periodic_line2d::__iterator, mln::point2d, mln::point2d>
    {
      __iterator(point2d delta, int k)
        : m_delta(delta)
        , m_k(k)
      {
      }

      void init()
      {
        m_i     = 0;
        m_point = m_delta * (-m_k);
      }
      void next()
      {
        m_point += m_delta;
        m_i++;
      }
      bool    finished() const { return m_i == (2 * m_k + 1); }
      point2d dereference() const { return m_point; }

    private:
      point2d m_delta;
      int     m_k;
      point2d m_point;
      int     m_i;
    };

  } // namespace se

} // namespace mln

