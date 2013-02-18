#ifndef MLN_CORE_RANGE_TRANSFORM_HPP
# define MLN_CORE_RANGE_TRANSFORM_HPP

# include <mln/core/iterator/transform_iterator.hpp>

namespace mln
{

  template <typename InputRange, typename UnaryFunction>
    struct transformed_range
    {
      typedef transform_iterator<typename InputRange::iterator, UnaryFunction>		iterator;
      typedef transform_iterator<typename InputRange::const_iterator, UnaryFunction>	const_iterator;
      typedef typename iterator::value_type				value_type;
      typedef typename iterator::reference				reference;

      filtered_range(InputRange& rng, const UnaryFunction& fun)
	: m_rng (rng), m_fun(fun)
      {
      }

      const_iterator iter() const
      {
	return const_iterator( m_rng.iter(), m_fun );
      }

      iterator iter()
      {
	return iterator( m_rng.iter(), m_fun );
      }

      bool has(const value_type& v)
      {
	return m_pred(v) and m_rng.has(v);
      }

      std::size_t size() const
      {
	return m_rng.size();
      }

    private:
      InputRange&	m_rng;
      UnaryFunction     m_fun;
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
