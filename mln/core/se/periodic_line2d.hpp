#ifndef MLN_CORE_SE_PERIODIC_LINE_2D_HPP
# define MLN_CORE_SE_PERIODIC_LINE_2D_HPP

# include <mln/core/neighborhood/dyn_neighborhood.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/point.hpp>

namespace mln
{

  namespace se
  {

    struct periodic_line2d : dyn_neighborhood_base<dynamic_neighborhood_tag, periodic_line2d>
    {
      struct __iterator;
      using iterator = __iterator;
      using const_iterator = __iterator;
      using is_incremental = std::false_type;
      using is_decomposable = std::false_type;


      periodic_line2d(point2d period, int k);
      iterator offsets() const;

      int size() const { return 2 * m_k + 1; }

      int repetition() const { return m_k; }
      point2d period() const { return m_delta; }

    private:
      point2d m_delta;
      int m_k;
    };


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    struct periodic_line2d::__iterator : iterator_base<periodic_line2d::__iterator, mln::point2d, mln::point2d>
    {
      __iterator(point2d delta, int k)
        : m_delta(delta),
          m_k(k)
        {
        }

      void init() { m_i = 0; m_point = m_delta * (-m_k); }
      void next() { m_point += m_delta; m_i++; }
      bool finished() const { return m_i == (2 * m_k + 1); }
      point2d dereference() const { return m_point; }

    private:
      point2d m_delta;
      int m_k;
      point2d m_point;
      int m_i;
    };

    inline
    periodic_line2d::periodic_line2d(point2d delta, int k)
    {
      mln_precondition((k >= 0) && "The extent must be positive");

      m_delta = (delta < mln::point2d{0,0}) ? point2d(-delta) : delta;
      m_k = k;
    }

    inline
    periodic_line2d::iterator periodic_line2d::offsets() const
    {
      return iterator(m_delta, m_k);
    }

  }

} // end of namespace mln

#endif //!MLN_CORE_SE_PERDIODIC_LINE_2D_HPP
