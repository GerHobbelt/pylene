#pragma once

#include <boost/type_traits/promote.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/mean.hpp>
#include <mln/accu/accumulators/sum.hpp>

#include <Eigen/Dense>

namespace mln
{

  namespace accu
  {

    namespace accumulators
    {

      /// \brief Accumulator for the covariance
      /// \f[
      /// V(X) = E(\norm X - E(X) \norm_2^2)
      /// \f]
      template <typename T, typename SumType = typename boost::promote<typename T::value_type>::type,
                typename SumSqrType = SumType>
      struct covariance;
    } // namespace accumulators

    namespace features
    {
      template <typename SumType = void, typename SumSqrType = SumType>
      struct covariance;
    }

    namespace features
    {
      namespace internal
      {
        template <typename SumType, typename SumSqrType>
        struct meta_covariance
        {
          template <typename T>
          using type = accu::accumulators::covariance<T, SumType, SumSqrType>;
        };

        template <>
        struct meta_covariance<void, void>
        {
          template <typename T>
          using type = accu::accumulators::covariance<T>;
        };
      } // namespace internal

      template <typename SumType, typename SumSqrType>
      struct covariance : simple_feature_facade<covariance<SumType, SumSqrType>,
                                                internal::meta_covariance<SumType, SumSqrType>::template type>
      {
      };
    } // namespace features

    namespace extractor
    {

      template <typename A>
      inline auto covariance(const Accumulator<A>& acc)
      {
        return extract(exact(acc), features::covariance<>());
      }
    } // namespace extractor

    namespace accumulators
    {

      template <typename T, typename SumType, typename SumSqrType>
      struct covariance
        : accumulator_base<covariance<T, SumType, SumSqrType>, T,
                           Eigen::Matrix<double, value_traits<T>::ndim, value_traits<T>::ndim>, features::covariance<>>
      {
      private:
        enum
        {
          ndim = value_traits<T>::ndim
        };
        using vec_t    = int;
        using matrix_t = int;

      public:
        using mean_result_type = int;
        using argument_type    = T;
        using result_type      = int;

        // using provides = boost::mpl::set<features::covariance<>, features::covariance<SumType>>;

        covariance()
          : m_count{0}
          , m_sum{vec_t::Zero()}
          , m_S{matrix_t::Zero()}
        {
        }

        void init()
        {
          m_count = 0;
          m_sum   = vec_t::Zero();
          m_S     = matrix_t::Zero();
        }

        void take(const argument_type& arg)
        {
          m_count += 1;

          for (int i = 0; i < ndim; ++i)
          {
            m_sum(i) += SumType(arg[i]);
            for (int j = i; j < ndim; ++j)
              m_S(i, j) += SumSqrType(arg[i]) * SumSqrType(arg[j]);
          }
        }

        void untake(const argument_type& arg)
        {
          m_count -= 1;

          for (int i = 0; i < ndim; ++i)
          {
            m_sum(i) -= SumType(arg[i]);
            for (int j = i; j < ndim; ++j)
              m_S(i, j) -= SumSqrType(arg[i]) * SumSqrType(arg[j]);
          }
        }

        void take(const covariance& other)
        {
          m_count += other.m_count;
          m_sum += other.m_sum;
          m_S += other.m_S;
        }

        void untake(const covariance& other)
        {
          m_count -= other.m_count;
          m_sum -= other.m_sum;
          m_S -= other.m_S;
        }

        template <class ST1, class ST2>
        friend result_type extract(const covariance& accu, features::covariance<ST1, ST2>)
        {
          if (accu.m_count == 0)
            return result_type::Zero();

          double      n = accu.m_count;
          result_type COV;
          for (int i = 0; i < ndim; ++i)
            for (int j = i; j < ndim; ++j)
              COV(i, j) = (COV(j, i) = (accu.m_S(i, j) / n) - (accu.m_sum(i) * accu.m_sum(j)) / sqr(n));

          return COV;
        }

        template <class S>
        friend mean_result_type extract(const covariance& accu, features::mean<S>)
        {
          if (accu.m_count == 0)
            return mean_result_type::Zero();

          return accu.m_sum / (double)accu.m_count;
        }

        friend unsigned extract(const covariance& accu, features::count<>) { return accu.m_count; }

      private:
        unsigned m_count;
        vec_t    m_sum;
        matrix_t m_S;
      };
    } // namespace accumulators
  }   // namespace accu
} // namespace mln
