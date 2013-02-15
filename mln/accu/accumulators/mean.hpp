#ifndef MLN_ACCU_ACCUMULATOTS_MEAN_HPP
# define MLN_ACCU_ACCUMULATOTS_MEAN_HPP

# include <mln/accu/accumulators/sum.hpp>
# include <mln/accu/accumulators/count.hpp>
# include <mln/accu/accumulator_base.hpp>

namespace mln
{

  namespace accu
  {

    namespace accumulators
    {
      template <typename T, typename SumType = decltype( std::declval<T>() + std::declval<T>() )  >
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
      inline
      typename A::return_type
      mean(const Accumulator<A>& acc)
      {
	return extract(exact(acc), features::mean<> ());
      }

    }


    namespace features
    {
      template <typename SumType>
      struct mean : feature_base< mean<SumType> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::mean<T, SumType> type;
	};
      };

      template <>
      struct mean<void> : feature_base< mean<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::mean<T> type;
	};
      };

      template <typename SumType>
      struct depends
      {
        typedef boost::mpl::set< sum<SumType>, count > type;
      };

    }

   namespace accumulators
    {

      template <typename T, typename SumType>
      struct mean : composite_accumulator_base< mean<T, SumType>, T, sum<T, SumType>, count<> >
      {
	typedef T	argument_type;
	typedef SumType return_type;

	typedef features::mean<> feature;

	friend
	SumType extract(const mean& accu, features::mean<> )
	{
	  return extractor::sum(accu) / extractor::count(accu);
	}

      private:
	SumType m_sum;
      };

    }

  }

}

#endif // ! MLN_ACCU_ACCUMULATOTS_MEAN_HPP
