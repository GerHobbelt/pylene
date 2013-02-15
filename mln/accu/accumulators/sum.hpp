#ifndef MLN_ACCU_ACCUMULATOTS_SUM_HPP
# define MLN_ACCU_ACCUMULATOTS_SUM_HPP

/// \file
/// FIXME: use literal::zero instead of default initialization

# include <mln/accu/accumulator.hpp>
# include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {

      template <typename T, typename SumType = decltype( std::declval<T>() + std::declval<T>() )>
      struct sum;
    }

    namespace features
    {
      template <typename SumType = void>
      struct sum;
    }

    namespace extractor
    {

      template <typename A>
      typename A::return_type
      sum(const Accumulator<A>& acc)
      {
	return extract(exact(acc), features::sum<> ());
      }

    }


    namespace features
    {

      template <typename SumType>
      struct sum : feature_base<SumType>
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::sum<T, SumType> type;
	};
      };

      template <>
      struct sum<void> : feature_base< sum<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::sum<T> type;
	};
      };
    }

    namespace accumulators
    {

      template <typename T, typename SumType>
      struct sum : Accumulator< sum<T, SumType> >
      {
	typedef T		argument_type;
	typedef SumType		return_type;
	typedef boost::mpl::set< features::sum<> > provides;

	sum()
	  : m_sum ( SumType() )
	{
	}

	void init()
	{
	  m_sum = SumType();
	}

	void take(const T& v)
	{
	  m_sum += v;
	}

	void untake(const T& v)
	{
	  m_sum -= v;
	}

	template <typename Other>
	void take(const Accumulator<Other>& other)
	{
	  m_sum += extractor::sum(other);
	}

	friend
	SumType extract(const sum& accu, features::sum<> )
	{
	  return accu.m_sum;
	}

      private:
	SumType m_sum;
      };

    }

  }

}

#endif // !MLN_ACCU_ACCUMULATOTS_SUM_HPP
