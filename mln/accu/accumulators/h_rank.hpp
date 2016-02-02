#ifndef MLN_ACCU_ACCUMULATORS_H_RANK_HPP
# define MLN_ACCU_ACCUMULATORS_H_RANK_HPP

# include <mln/accu/accumulator_base.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/value/indexer.hpp>
# include <array>

/// \file

namespace mln
{

  namespace accu
  {

    namespace accumulators
    {
      /// \brief Compute the rank value of some set.
      ///
      /// Compute the rank value (defined by \p Ratio) of some set.
      /// The value 𝑟 of rank 𝑅 of a set 𝑆 of size 𝑛 is
      /// defined as:
      ///
      /// \f[
      /// r = \bigwedge \{ x \in S \, \mid \, |\{y \in S, y \le x \}| > \lfloor n * R \rfloor \}
      /// \f]
      ///
      /// \tparam T Value type of the set.
      /// \tparam Ratio A static std::ratio<Numerator,Denomitator> type.
      ///
      /// \note This accumulator is specialized for Boolean type.
      template <class T, class Ratio>
      struct h_rank;
    }

    namespace features
    {
      /// \brief Feature for computing the rank value of some set.
      /// \tparam Ratio A static std::ratio<Numerator,Denomitator> type.
      template <class Ratio>
      struct h_rank;
    }

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace features
    {

      template <class Ratio>
      struct h_rank : simple_feature< h_rank<Ratio> >
      {
        template <typename T>
        struct apply
        {
          typedef typename accumulators::h_rank<T, Ratio> type;
        };

        template <typename T>
        accumulators::h_rank<T, Ratio>
        make() const
        {
          return accumulators::h_rank<T, Ratio> ();
        }
      };

    }

    namespace extractor
    {

      template <typename Ratio, typename A>
      auto
      h_rank (const Accumulator<A>& acc)
        -> decltype( extract(exact(acc), std::declval< features::h_rank<Ratio> > ()) )
      {
        return extract(exact(acc), features::h_rank<Ratio> ());
      }

    }

    namespace accumulators
    {
      // Generic impl for low-quantized types
      template <class T, class Ratio>
      struct h_rank : accumulator_base< h_rank<T, Ratio>, T, T, features::h_rank<Ratio> >
      {
        typedef T argument_type;
        typedef T result_type;
        typedef boost::mpl::set< features::h_rank<Ratio> > provides;
        typedef std::true_type has_untake;

        static_assert(std::is_unsigned<T>::value, "T must be unsigned");
        static_assert(value_traits<T>::quant <= 16, "T must be low quantized");
        static_assert(Ratio::num < Ratio::den, "Invalid ratio (must be [0,1)");

        h_rank() :
          m_hist {{0,}},
          m_count {0},
          m_cumsum {0},
          m_current {0}
        {
        }

        void init()
        {
          m_hist.fill(0);
          m_count = 0;
          m_cumsum = 0;
          m_current = 0;
        }

        void take(const argument_type& x)
        {
          ++m_hist[x];
          ++m_count;
          if (x <= m_current)
            ++m_cumsum;
          m_update();
        }

        // FIXME: take(const Accumulator<Accu>& other)
        // This impl is not generic enough to composite accumulator.

        void take(const h_rank& other)
        {
          m_count += other.m_count;
          for (int i = 0; i < m_hist.size(); ++i)
            m_hist[i] += other.m_hist[i];
          m_update_full();
        }

        void untake(const argument_type& x)
        {
          mln_precondition(m_hist[x] > 0);
          --m_hist[x];
          --m_count;
          if (x <= m_current)
            --m_cumsum;
          m_update();
        }


        friend result_type extract(const h_rank& accu, features::h_rank<Ratio>)
        {
          return accu.m_current;
        }


      private:
        void m_update()
        {
          unsigned P = m_count * Ratio::num / Ratio::den;
          if (m_cumsum <= P) {
            mln_assertion(m_current < (int) (m_hist.size() - 1));
            while (m_hist[++m_current] == 0)
              ;
            m_cumsum += m_hist[m_current];
          }

          unsigned Q = m_count - P;
          unsigned EQ = m_count - m_cumsum + m_hist[m_current]; // P[y >= r]
          if (EQ < Q) {
            mln_assertion(m_current > 0);
            m_cumsum -= m_hist[m_current];
            while (m_hist[--m_current] == 0)
              ;
          }

          mln_assertion(m_cumsum > P);
          mln_assertion( (m_count - m_cumsum + m_hist[m_current]) >= Q );
        }

        void m_update_full()
        {
          unsigned P = m_count * Ratio::num / Ratio::den;
          m_cumsum = m_hist[0];
          m_current = 0;
          while (m_cumsum <= P) {
            ++m_current;
            m_cumsum += m_hist[m_current];
          }

          mln_assertion(m_cumsum > P);
        }


      private:
        std::array<unsigned, indexer<T, std::less<T>>::nvalues> m_hist;
        unsigned m_count;  // Number of value in the set
        unsigned m_cumsum; // Number of value in the set less or equal to r
        int m_current;     // Current position of the rank value r
      };


      // Impl for boolean
      template <class Ratio>
      struct h_rank<bool, Ratio> : accumulator_base< h_rank<bool, Ratio>, bool, bool, features::h_rank<Ratio> >
      {
        typedef bool argument_type;
        typedef bool result_type;
        typedef boost::mpl::set< features::h_rank<Ratio> > provides;
        typedef std::true_type has_untake;


        static_assert(Ratio::num < Ratio::den, "Invalid ratio (must be [0,1)");

        h_rank() :
          m_count {0},
          m_false {0}
        {
        }

        void init()
        {
          m_count = 0;
          m_false = 0;
        }

        void take(const argument_type& x)
        {
          m_false += (not x);
          ++m_count;
        }

        // FIXME: take(const Accumulator<Accu>& other)
        // This impl is not generic enough to composite accumulator.

        void take(const h_rank& other)
        {
          m_false += other.m_false;
          m_count += other.m_count;
        }

        void untake(const argument_type& x)
        {
          mln_precondition(m_count > 0);
          m_false -= (not x);
          --m_count;
        }


        friend result_type extract(const h_rank& accu, features::h_rank<Ratio>)
        {
          return not ( (accu.m_count * Ratio::num / Ratio::den) < accu.m_false );
        }


      private:
        unsigned m_count;  // Number of value in the set
        unsigned m_false;  // Number of false value in the set
      };


    }

  }

}

# endif //! MLN_ACCU_ACCUMULATORS_H_RANK_HPP
