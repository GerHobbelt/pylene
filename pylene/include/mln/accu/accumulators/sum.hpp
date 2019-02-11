#pragma once

/// \file
/// FIXME: use literal::zero instead of default initialization

#include <mln/accu/accumulator_base.hpp>

#include <utility>


namespace mln
{

  namespace accu
  {

    namespace accumulators
    {
      /// \brief Summation accumulator.
      /// \tparam T The type of the arguments to sum up.
      /// \tparam SumType The accumulation type (by default, the type of `T + T` w.r.t C++ type promotions)
      template <typename T, typename SumType = decltype(std::declval<T>() + std::declval<T>())>
      struct sum;
    } // namespace accumulators

    namespace features
    {

      /// \brief Summation accumulator.
      /// \tparam SumType The accumulation type (`void` will use the default type).
      template <typename SumType = void>
      struct sum;
    } // namespace features

    namespace extractor
    {

      template <typename A>
      auto sum(const Accumulator<A>& acc);
    }

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    namespace features
    {
      namespace internal
      {
        template <typename SumType>
        struct meta_sum
        {
          template <typename T>
          using type = accu::accumulators::sum<T, SumType>;
        };

        template <>
        struct meta_sum<void>
        {
          template <typename T>
          using type = accu::accumulators::sum<T>;
        };
      } // namespace internal

      template <typename SumType>
      struct sum : simple_feature_facade<sum<SumType>, internal::meta_sum<SumType>::template type>
      {
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      auto sum(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::sum<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename T, typename SumType>
      struct sum : accumulator_base<sum<T, SumType>, T, SumType, features::sum<>>
      {
        typedef T                                argument_type;
        typedef SumType                          result_type;
        typedef boost::mpl::set<features::sum<>> provides;
        typedef std::true_type                   has_untake;

        sum()
          : m_sum(SumType())
        {
        }

        void init() { m_sum = SumType(); }

        void take(const T& v) { m_sum += (SumType)v; }

        void untake(const T& v) { m_sum -= (SumType)v; }

        template <typename Other>
        void take(const Accumulator<Other>& other)
        {
          m_sum += extractor::sum(other);
        }

        friend SumType extract(const sum& accu, features::sum<>) { return accu.m_sum; }

      private:
        SumType m_sum;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
