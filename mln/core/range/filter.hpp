#ifndef MLN_CORE_RANGE_FILTER_HPP
# define MLN_CORE_RANGE_FILTER_HPP

# include <mln/core/iterator/filter_iterator.hpp>

namespace mln
{

    template <typename InputRange, typename Predicate>
    struct filtered_range
    {
      typedef filter_iterator<typename InputRange::iterator, Predicate>		iterator;
      typedef filter_iterator<typename InputRange::const_iterator, Predicate>	const_iterator;
      typedef typename iterator::value_type				value_type;
      typedef typename iterator::reference				reference;

      filtered_range(InputRange& rng, const Predicate& pred)
	: m_rng (rng), m_pred(pred)
      {
      }

      const_iterator iter() const
      {
	return const_iterator( m_rng.iter(), m_pred );
      }

      iterator iter()
      {
	return iterator( m_rng.iter(), m_pred );
      }

      bool has(const value_type& v)
      {
	return m_pred(v) and m_rng.has(v);
      }

    private:
      InputRange& m_rng;
      Predicate   m_pred;
    };



  namespace rng
  {

    template <typename InputRange, typename Predicate>
    filtered_range<InputRange, Predicate>
    filter(InputRange& rng, Predicate pred)
    {
      return filtered_range<InputRange, Predicate>(rng, pred);
    }

  } // end of namespace mln::rng

} // end of namespace mln

#endif // ! MLN_CORE_RANGE_FILTER_HPP
