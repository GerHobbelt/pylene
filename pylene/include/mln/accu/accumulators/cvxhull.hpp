#pragma once

#include <mln/accu/accumulator.hpp>
#include <mln/accu/accumulator_base.hpp>
#include <mln/accu/accumulators/cvxhull_impl.hpp>
#include <mln/core/assert.hpp>

namespace mln
{

  namespace accu
  {
    // FWD
    namespace accumulators
    {
      template <typename P>
      struct cvxhull;
    }

    namespace features
    {

      struct cvxhull : simple_feature<cvxhull>
      {
        template <typename P>
        struct apply
        {
          typedef accumulators::cvxhull<P> type;
        };

        template <typename P>
        accumulators::cvxhull<P> make() const
        {
          return accumulators::cvxhull<P>();
        }
      };

      struct pvector : simple_feature<pvector>
      {
        template <typename P>
        struct apply
        {
          typedef accumulators::cvxhull<P> type;
        };

        template <typename P>
        accumulators::cvxhull<P> make() const
        {
          return accumulators::cvxhull<P>();
        }
      };
    } // namespace features

    namespace extractor
    {
      template <typename A>
      auto cvxhull(const Accumulator<A>& accu) -> decltype(extract(accu, features::cvxhull()))
      {
        return extract(accu, features::cvxhull());
      }

      template <typename A>
      auto pvector(const Accumulator<A>& accu) -> decltype(extract(accu, features::pvector()))
      {
        return extract(accu, features::pvector());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename P>
      struct cvxhull : accumulator_base<cvxhull<P>, P, std::vector<P>, features::cvxhull>
      {
        typedef P                        argument_type;
        typedef std::vector<P>           result_type;
        typedef features::cvxhull        feature;
        typedef boost::mpl::set<feature> provides;

        cvxhull() {}

        void init()
        {
          m_points.clear();
          m_newline = true;
        }

        void take(const P& p)
        {
          mln_precondition(m_points.empty() || m_points.back() < p);
          if (m_points.empty() or m_points.back().y() != p.y())
          {
            m_points.push_back(p);
            m_newline = true;
          }
          else if (m_newline)
          {
            m_points.push_back(p);
            m_newline = false;
          }
          else
          {
            m_points.back() = p;
          }
        }

        template <typename A>
        void take(const Accumulator<A>& other)
        {
          std::vector<P>& ovec = extractor::pvector(other);
          std::vector<P>  aux(m_points.size() + ovec.size());
          std::merge(m_points.begin(), m_points.end(), ovec.begin(), ovec.end(), aux.begin());
          m_points = aux;
        }

        friend std::vector<P>& extract(cvxhull& accu, features::pvector) { return accu.m_points; }

        friend std::vector<P> extract(const cvxhull& accu, features::cvxhull)
        {
          std::vector<P> pts = convexhull(accu.m_points.data(), accu.m_points.size());
          return pts;
        }

      private:
        mutable        std::vector<P> m_points;
        bool           m_newline;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
