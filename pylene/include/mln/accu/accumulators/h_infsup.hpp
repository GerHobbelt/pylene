#pragma once

#include <mln/accu/accumulator_base.hpp>
#include <mln/accu/accumulators/infsup.hpp>
#include <mln/core/value/indexer.hpp>
#include <mln/core/value/value_traits.hpp>

#include <concepts>

#ifdef _MSC_VER
#pragma warning(disable : 4800)
#endif

/// FIXME: use indexer / rename has histogram

namespace mln
{

  namespace accu
  {

    namespace accumulators
    {
      /// \brief An accumulator that computes the infimum of values
      /// based on the tracking of values. It provides
      template <class T>
      struct h_inf;

      template <class T>
      struct h_sup;
    } // namespace accumulators

    namespace features
    {
      struct h_inf;
      struct h_sup;
    } // namespace features

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace features
    {

      struct h_inf : simple_feature_facade<h_inf, accumulators::h_inf>
      {
      };

      struct h_sup : simple_feature_facade<h_sup, accumulators::h_sup>
      {
      };
    } // namespace features

    namespace accumulators
    {

      template <typename E, typename T, typename F>
      requires(std::integral<T>and value_traits<T>::quant <= 16) //
          struct h_infsup_base : accumulator_base<E, T, T, F>
      {
        using argument_type = T;
        using result_type   = T;
        using provides      = boost::mpl::set<features::h_sup, features::h_inf, features::inf<>, features::sup<>>;

        using has_untake = std::true_type;

        constexpr h_infsup_base()
          : m_inf(value_traits<T>::sup())
          , m_sup(value_traits<T>::inf())
          , m_count(0)
          , m_hist{{
                0,
            }}
        {
        }

        void init()
        {
          m_inf   = value_traits<T>::sup();
          m_sup   = value_traits<T>::inf();
          m_count = 0;
          m_hist.fill(0);
        }

        void take(const h_infsup_base& other)
        {
          for (std::size_t i = 0; i < m_hist.size(); ++i)
            m_hist[i] += other.m_hist[i];

          m_count += other.m_count;
          m_inf = std::min(other.m_inf, m_inf);
          m_sup = std::max(other.m_sup, m_sup);
        }


        void take(const T& x)
        {
          ++m_hist[x];
          ++m_count;
          if (x < m_inf)
            m_inf = x;
          if (x > m_sup)
            m_sup = x;
        }

        void untake(const T& x)
        {
          mln_precondition(m_count > 0);
          mln_precondition(m_hist[x] > 0);
          --m_hist[x];
          --m_count;

          if (m_hist[x] == 0)
          {
            if (m_count == 0)
            {
              m_inf = value_traits<T>::sup();
              m_sup = value_traits<T>::inf();
            }
            else if (x == m_inf)
            {
              int i = m_inf;
              while (not m_hist[i])
                ++i;
              m_inf = i;
            }
            else if (x == m_sup)
            {
              int i = m_sup;
              while (not m_hist[i])
                --i;
              m_sup = i;
            }
          }
        }

        friend T extract(const h_infsup_base& accu, features::inf<>) { return accu.m_inf; }

        friend T extract(const h_infsup_base& accu, features::sup<>) { return accu.m_sup; }

      protected:
        T                                                       m_inf;
        T                                                       m_sup;
        unsigned                                                m_count;
        std::array<unsigned, indexer<T, std::less<T>>::nvalues> m_hist;
      };

      template <typename T>
      struct h_inf : h_infsup_base<h_inf<T>, T, features::inf<>>
      {
      };

      template <typename T>
      struct h_sup : h_infsup_base<h_sup<T>, T, features::sup<>>
      {
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
