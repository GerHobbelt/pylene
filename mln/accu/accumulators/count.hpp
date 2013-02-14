#ifndef MLN_ACCU_ACCUMULATOTS_COUNT_HPP
# define MLN_ACCU_ACCUMULATOTS_COUNT_HPP

# include <mln/accu/accumulator.hpp>
# include <mln/accu/dontcare.hpp>
# include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {
      template <typename CountType = std::size_t>
      struct count;
    }

    namespace features
    {
      template <typename CountType = std::size_t>
      struct count;
    }

    namespace extractor
    {

      template <typename A>
      inline
      typename A::return_type
      count(const Accumulator<A>& acc)
      {
	return extract(exact(acc), features::count<> ());
      }

    }


    namespace features
    {

      template <typename CountType>
      struct count : feature_base< count<CountType> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::count<CountType> type;
	};
      };

    }

    namespace accumulators
    {

      template <typename CountType>
      struct count : Accumulator< count<CountType> >
      {
	typedef dontcare		argument_type;
	typedef CountType		return_type;
	typedef features::count<>	feature;

	count()
	  : m_count (0)
	{
	}

	void init()
	{
	  m_count = 0;
	}

	void take(const dontcare&)
	{
	  ++m_count;
	}

	void untake(const dontcare&)
	{
	  --m_count;
	}

	template <typename Other>
	void take(const Accumulator<Other>& other)
	{
	  m_count += extractor::count(other);
	}

	friend
	return_type extract(const count& accu, features::count<> )
	{
	  return accu.m_count;
	}

      private:
	CountType m_count;
      };

    }

  }

}

#endif // !MLN_ACCU_ACCUMULATOTS_COUNT_HPP
