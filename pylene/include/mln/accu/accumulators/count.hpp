#pragma once

/// \file
/// \brief Header file for counting accumulator.

#include <mln/accu/accumulator_base.hpp>
#include <mln/core/utils/dontcare.hpp>
#include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {
      /// \brief The counting accumulator.
      /// \tparam CountType is underlying the counter type (it must be incrementable,
      ///         and decrementable.
      template <typename CountType = std::size_t>
      struct count;
    } // namespace accumulators

    namespace features
    {
      template <typename CountType = std::size_t>
      struct count;
    }

    namespace extractor
    {

      template <typename A>
      inline auto count(const Accumulator<A>& acc);
    }

    namespace features
    {
      template <typename CountType>
      struct count : simple_feature<count<CountType>>
      {
        template <typename T>
        struct apply
        {
          using type = accumulators::count<CountType>;
        };

        template <typename T>
        accumulators::count<CountType> make() const
        {
          return accumulators::count<CountType>();
        }
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      inline auto count(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::count<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename CountType>
      struct count : accumulator_base<count<CountType>, dontcare_t, CountType, features::count<>>
      {
        using argument_type = dontcare_t;
        using result_type   = CountType;
        using provides      = boost::mpl::set<features::count<>>;
        using has_untake    = std::true_type;

        count()
          : m_count(0)
        {
        }

        void init() { m_count = 0; }

        void take(dontcare_t) { ++m_count; }

        void untake(dontcare_t) { --m_count; }

        template <typename Other>
        void take(const Accumulator<Other>& other)
        {
          m_count += extractor::count(other);
        }

        friend result_type extract(const count& accu, features::count<>) { return accu.m_count; }

      private:
        CountType m_count;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
