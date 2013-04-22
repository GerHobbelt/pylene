#ifndef MLN_ACCU_ACCUMULATORS_MIN_HPP
# define MLN_ACCU_ACCUMULATORS_MIN_HPP

/// \file
/// FIXME: use literal::zero instead of default initialization

# include <mln/accu/accumulator_base.hpp>
# include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {

      template <typename T, typename Compare = std::less<T> >
      struct min;
    }

    namespace features
    {
      template <typename Compare = void>
      struct min;
    }

    namespace extractor
    {

      template <typename A>
      typename A::return_type
      min(const Accumulator<A>& acc)
      {
	return extract(exact(acc), features::min<> ());
      }

    }


    namespace features
    {

      template <typename Compare>
      struct min : simple_feature< min<Compare> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::min<T, Compare> type;
	};
      };

      template <>
      struct min<void> : simple_feature< min<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::sum<T, std::less<T> > type;
	};
      };
    }

    namespace accumulators
    {

      template <typename T, typename Compare>
      struct min : accumulator_base< min<T, Compare>, T, T, features::min<> >
      {
	typedef T		argument_type;
	typedef T		return_type;
	typedef features::min<> feature;

	min(const Compare& cmp = Compare())
	  : m_val( value_traits<T, Compare>::max() ),
	    m_cmp( cmp )
	{
	}

	void init()
	{
	  m_val = value_traits<T, Compare>::max();
	}

	void take(const T& v)
	{
	  if (m_cmp(v, m_val))
	    m_val = v;
	}

	template <typename Other>
	void take(const Accumulator<Other>& other)
	{
	  T v = extractor::min(other);
	  if (m_cmp(v, m_val))
	    m_val = v;
	}

	friend
	T extract(const min& accu, features::min<> )
	{
	  return accu.m_val;
	}

      private:
	T	m_val;
	Compare m_cmp;
      };

    }

  }

}

#endif // !MLN_ACCU_ACCUMULATORS_MIN_HPP