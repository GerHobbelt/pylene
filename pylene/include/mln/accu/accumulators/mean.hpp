#pragma once

#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/sum.hpp>
#include <mln/accu/composite_accumulator.hpp>


namespace mln
{

  namespace accu
  {

    namespace accumulators
    {
      template <typename T, typename SumType = decltype(std::declval<T>() + std::declval<T>())>
      struct mean;
    }

    namespace features
    {
      template <typename SumType = void>
      struct mean;
    }

    namespace extractor
    {

      template <typename A>
      inline auto mean(const Accumulator<A>& acc);
    }

    namespace features
    {
      template <typename SumType>
      struct mean : simple_feature<mean<SumType>>
      {
        template <typename T>
        struct apply
        {
          using type = accumulators::mean<T, SumType>;
        };

        template <typename T>
        accumulators::mean<T, SumType> make() const
        {
          return accumulators::mean<T, SumType>();
        }
      };

      template <>
      struct mean<void> : simple_feature<mean<void>>
      {
        template <typename T>
        struct apply
        {
          using type = accumulators::mean<T>;
        };

        template <typename T>
        accumulators::mean<T> make() const
        {
          return accumulators::mean<T>();
        }
      };

      template <typename SumType>
      struct depends<mean<SumType>>
      {
        using type = boost::mpl::set<sum<SumType>, count<>>;
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      inline auto mean(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::mean<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename T, typename SumType>
      struct mean : composite_accumulator_facade<mean<T, SumType>, T, SumType, features::mean<SumType>>
      {
        using argument_type = T;
        using result_type   = SumType;
        using provides      = boost::mpl::set<features::mean<>, features::mean<SumType>>;

        friend SumType extract(const mean& accu, features::mean<SumType>) { return extract(accu, features::mean<>()); }

        friend SumType extract(const mean& accu, features::mean<>)
        {
          auto v = extractor::count(accu);
          if (v == 0)
            return extractor::sum(accu);
          else
            return static_cast<SumType>(extractor::sum(accu) / extractor::count(accu));
        }
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
