#pragma once

/// \file
/// \brief Header file for the maximum accumulator

#include <mln/accu/accumulator.hpp>
#include <mln/accu/accumulator_base.hpp>
#include <mln/core/value/value_traits.hpp>

#include <utility>


namespace mln
{

  namespace accu
  {
    namespace accumulators
    {

      /// \brief Maximum accumlator.
      /// \tparam T The argument and result type.
      /// \tparam Compare The comparison function.
      template <typename T, typename Compare = std::less<T>>
      struct max;
    } // namespace accumulators

    namespace features
    {
      /// \brief Maximum accumlator.
      /// \tparam T The argument and result type.
      /// \tparam Compare The comparison function (`void` results in std::less by default).
      template <typename Compare = void>
      struct max;
    } // namespace features

    namespace extractor
    {

      template <typename A>
      auto max(const Accumulator<A>& acc);
    }

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace features
    {
      template <typename Compare>
      struct max : simple_feature<max<Compare>>
      {
        max(const Compare& cmp = Compare())
          : m_cmp(cmp)
        {
        }

        template <typename T>
        struct apply
        {
          using type = accumulators::max<T, Compare>;
        };

        template <typename T>
        accumulators::max<T, Compare> make() const
        {
          return accumulators::max<T, Compare>(m_cmp);
        }

      private: // dynamic parameter.
        Compare m_cmp;
      };

      namespace internal
      {
        template <typename T>
        using meta_max = accumulators::max<T>;
      }

      template <>
      struct max<void> : simple_feature_facade<max<void>, internal::meta_max>
      {
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      auto max(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::max<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename T, typename Compare>
      struct max : accumulator_base<max<T, Compare>, T, T, features::max<>>
      {
        using argument_type = T;
        using return_type   = T;
        // using feature = features::max<>;

        max(const Compare& cmp = Compare())
          : m_val(value_traits<T, Compare>::min())
          , m_cmp(cmp)
        {
        }

        void init() { m_val = value_traits<T, Compare>::min(); }

        void take(const T& v)
        {
          if (m_cmp(m_val, v))
            m_val = v;
        }

        template <typename Other>
        void take(const Accumulator<Other>& other)
        {
          T v = extractor::max(other);
          if (m_cmp(m_val, v))
            m_val = v;
        }

        friend T extract(const max& accu, features::max<>) { return accu.m_val; }

      private:
        T       m_val;
        Compare m_cmp;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
