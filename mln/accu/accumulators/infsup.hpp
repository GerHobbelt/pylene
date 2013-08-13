#ifndef MLN_ACCU_INFSUP_HPP
# define MLN_ACCU_INFSUP_HPP

# include <mln/accu/accumulator_base.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/ops.hpp>
# include <utility>

namespace mln
{

  namespace accu
  {
    namespace accumulators
    {

      template <typename T, typename Compare = std::less<T> >
      struct infsup;
    }

    namespace features
    {
      template <typename Compare = void>
      struct inf;

      template <typename Compare = void>
      struct sup;
    }

    namespace extractor
    {

      template <typename A>
      auto
      inf(const Accumulator<A>& acc)
	-> decltype( extract(exact(acc), features::inf<> ()) )
      {
	return extract(exact(acc), features::inf<> ());
      }

      template <typename A>
      auto
      sup(const Accumulator<A>& acc)
	-> decltype( extract(exact(acc), features::sup<> ()) )
      {
	return extract(exact(acc), features::sup<> ());
      }

    }


    namespace features
    {

      template <typename Compare>
      struct inf : simple_feature< inf<Compare> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::infsup<T, Compare> type;
	};
      };

      template <>
      struct inf<void> : simple_feature< inf<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::infsup<T, std::less<T> > type;
	};
      };

      template <typename Compare>
      struct sup : simple_feature< sup<Compare> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::infsup<T, Compare> type;
	};
      };

      template <>
      struct sup<void> : simple_feature< sup<void> >
      {
	template <typename T>
	struct apply
	{
	  typedef accumulators::infsup<T, std::less<T> > type;
	};
      };

    }

    namespace accumulators
    {

      template <typename T, typename Compare>
      struct infsup : Accumulator< infsup<T, Compare> >
      {
	typedef T				argument_type;
	typedef std::pair<T,T>			result_type;
	typedef boost::mpl::set<
	  features::inf<>, features::sup<> > provides;

	infsup(const Compare& cmp = Compare())
	  : m_inf( value_traits<T, Compare>::sup() ),
	    m_sup( value_traits<T, Compare>::inf() ),
	    m_cmp( cmp )
	{
	}


	void init()
	{
	  m_inf = value_traits<T, Compare>::sup();
	  m_sup = value_traits<T, Compare>::inf();
	}

	void take(const T& v)
	{
	  m_inf = inf(m_inf, v, m_cmp);
	  m_sup = sup(m_sup, v, m_cmp);
	}

	template <typename Other>
	void take(const Accumulator<Other>& other)
	{
	  T vinf = extractor::inf(other);
	  T vsup = extractor::sup(other);
	  m_inf = inf(m_inf, vinf, m_cmp);
	  m_sup = sup(m_sup, vsup, m_cmp);
	}


	std::pair<T, T> to_result() const
	{
	  return std::make_pair(m_inf, m_sup);
	}

	friend
	T extract(const infsup& accu, features::inf<> )
	{
	  return accu.m_inf;
	}

	friend
	T extract(const infsup& accu, features::sup<> )
	{
	  return accu.m_sup;
	}

      private:
	T	m_inf;
	T	m_sup;
	Compare m_cmp;
      };

    }

  }

}

#endif // ! MLN_ACCU_INFSUP_HPP
