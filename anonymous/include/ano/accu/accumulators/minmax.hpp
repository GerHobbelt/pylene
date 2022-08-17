#pragma once

/// \file
/// FIXME: use literal::zero instead of default initialization

#include <ano/accu/accumulator_base.hpp>
#include <ano/core/value/value_traits.hpp>
// Import min/max features
#include <ano/accu/accumulators/max.hpp>
#include <ano/accu/accumulators/min.hpp>

#include <utility>


namespace ano
{

  namespace accu
  {
    namespace accumulators
    {

      template <typename T, typename Compare = std::less<T>>
      struct minmax;
    }

    namespace features
    {
      template <typename Compare = void>
      struct minmax;
    }

    namespace extractor
    {

      template <typename A>
      auto minmax(const Accumulator<A>& acc);
    }

    namespace features
    {

      template <typename Compare>
      struct minmax : simple_feature<minmax<Compare>>
      {
        minmax(const Compare& cmp = Compare())
          : m_cmp(cmp)
        {
        }

        template <typename T>
        struct apply
        {
          typedef accumulators::minmax<T, Compare> type;
        };

        template <typename T>
        accumulators::minmax<T, Compare> make() const
        {
          return accumulators::minmax<T, Compare>(m_cmp);
        }

      private:
        Compare m_cmp;
      };

      template <>
      struct minmax<void> : simple_feature<minmax<void>>
      {
        template <typename T>
        struct apply
        {
          typedef accumulators::minmax<T> type;
        };

        template <typename T>
        accumulators::minmax<T> make() const
        {
          return accumulators::minmax<T>();
        }
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      auto minmax(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::minmax<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename T, typename Compare>
      struct minmax : Accumulator<minmax<T, Compare>>
      {
        using argument_type = T;
        using result_type   = std::pair<T, T>;
        using provides      = boost::mpl::set<features::min<>, features::max<>, features::minmax<>>;

        minmax(const Compare& cmp = Compare())
          : m_min(value_traits<T, Compare>::max())
          , m_max(value_traits<T, Compare>::min())
          , m_cmp(cmp)
        {
        }

        void init()
        {
          m_min = value_traits<T, Compare>::max();
          m_max = value_traits<T, Compare>::min();
        }

        void take(const T& v)
        {
          if (m_cmp(v, m_min))
            m_min = v;
          if (m_cmp(m_max, v))
            m_max = v;
        }

        template <typename Other>
        void take(const Accumulator<Other>& other)
        {
          T omin = extractor::min(other);
          T omax = extractor::max(other);
          if (m_cmp(omin, m_min))
            m_min = omin;
          if (m_cmp(m_max, omax))
            m_max = omax;
        }

        friend T extract(const minmax& accu, features::min<>) { return accu.m_min; }

        friend T extract(const minmax& accu, features::max<>) { return accu.m_max; }

        friend std::pair<T, T> extract(const minmax& accu, features::minmax<>)
        {
          return std::make_pair(accu.m_min, accu.m_max);
        }

        std::pair<T, T> to_result() const { return std::make_pair(m_min, m_max); }

      private:
        T       m_min;
        T       m_max;
        Compare m_cmp;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace ano
