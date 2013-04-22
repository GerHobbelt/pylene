#ifndef MLN_ACCU_ACCUMULATORS_MAX_HPP
# define MLN_ACCU_ACCUMULATORS_MAX_HPP

/// \file

# include <mln/accu/accumulator.hpp>
# include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {

      template <typename T, typename Compare = std::less<T> >
      struct max;
    }

    namespace features
    {
      template <typename Compare = void>
      struct max;
    }

    namespace extractor
    {

      template <typename A>
      typename A::return_type
      max(const Accumulator<A>& acc)
      {
	return extract(exact(acc), features::max<> ());
      }

    }


    namespace features
    {

      template <typename Compare>
      struct max : simple_feature< max<Compare> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::max<T, Compare> type;
	};
      };

      template <>
      struct max<void> : simple_feature< max<void> >
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
      struct max : accumulator_base< max<T, Compare>, T, T, feature::max<> >
      {
	typedef T		argument_type;
	typedef T		return_type;
	typedef features::max<> feature;

	max(const Compare& cmp = Compare())
	  : m_val( value_traits<T, Compare>::min() ),
	    m_cmp( cmp )
	{
	}

	void init()
	{
	  m_val = value_traits<T, Compare>::min();
	}

	void take(const T& v)
	{
	  if (m_cmp(v, m_val))
	    m_val = v;
	}

	template <typename Other>
	void take(const Accumulator<Other>& other)
	{
	  T v = extract::max(other);
	  if (m_cmp(v, m_val))
	    m_val = v;
	}

	friend
	T extract(const max& accu, features::max<> )
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

#endif // !MLN_ACCU_ACCUMULATORS_MAX_HPP