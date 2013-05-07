#ifndef MLN_ACCU_ACCUMULATOTS_MEAN_HPP
# define MLN_ACCU_ACCUMULATOTS_MEAN_HPP

# include <mln/accu/accumulators/sum.hpp>
# include <mln/accu/accumulators/count.hpp>
# include <mln/accu/composite_accumulator.hpp>

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
      struct mean : simple_feature< mean<SumType> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::mean<T, SumType> type;
	};
      };

      template <>
      struct mean<void> : simple_feature< mean<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::mean<T> type;
	};
      };

      template <typename SumType>
      struct depends< mean<SumType> >
      {
        typedef boost::mpl::set< sum<SumType>, count<> > type;
      };

    }

   namespace accumulators
    {

      template <typename T, typename SumType>
      struct mean : composite_accumulator_facade< mean<T, SumType>, T,  SumType, features::mean<SumType> >
      {
	typedef T	argument_type;
	typedef SumType return_type;

	typedef boost::mpl::set< features::mean<>, features::mean<SumType> > provides;

	friend
	SumType extract(const mean& accu, features::mean<SumType> )
	{
	  //std::string x = extract(exact(accu), features::count<> ());
	  //std::string y = extractor::count(accu);
	  return extractor::sum(accu) / extractor::count(accu);
	}

	friend
	SumType extract(const mean& accu, features::mean<> )
	{
	  auto v = extractor::count(accu);
	  if (v == 0)
	    return extractor::sum(accu);
	  else
	    return extractor::sum(accu) / extractor::count(accu);
	}


	//private:
	//SumType m_sum;
      };

    }

  }

}

#endif // ! MLN_ACCU_ACCUMULATOTS_MEAN_HPP
