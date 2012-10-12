#ifndef MLN_CORE_VALUE_INDEXER_HPP
# define MLN_CORE_VALUE_INDEXER_HPP

# include <type_traits>
# include <functional>

# include <mln/core/assert.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/value/index.hpp>

# include <boost/integer.hpp>

namespace mln
{

  template <typename V, typename StrictWeakOrdering, typename Enable = void>
  struct indexer;


  // Specialization for builtin unsigned integral type
  template <typename V>
  struct indexer<V, std::less<V>, typename std::enable_if<std::is_integral<V>::value and
							  std::is_unsigned<V>::value>::type>
  {
    static_assert(value_traits<V>::quant < value_traits<std::size_t>::quant,
		  "The type is not indexable");

    typedef Index<V, std::less<V> > index_type;
    typedef V value_type;
    static const bool inversible = true;
    static const std::size_t nvalues = value_traits<V>::sup + 1;


    index_type operator() (value_type x) const { return index_type(x); }
    value_type inv (index_type i) const { return i; }
  };

  template <typename V>
  struct indexer<V, std::greater<V>, typename std::enable_if<std::is_integral<V>::value and
							  std::is_unsigned<V>::value>::type>
  {
    static_assert(value_traits<V>::quant <= value_traits<std::size_t>::quant,
		  "The type is not indexable");

    typedef Index<V, std::greater<V> > index_type;
    typedef V value_type;
    static const bool inversible = true;

    index_type operator() (value_type x) const { return index_type(x); }
    value_type inv (index_type i) const { return i; }
  };


  // Specialization for builtin signed integral type
  // FIXME use the type mln::uint<n>
  template <typename V>
  struct indexer<V, std::less<V>, typename std::enable_if<std::is_integral<V>::value and
							  std::is_signed<V>::value>::type>
  {
    static_assert(value_traits<V>::quant <= value_traits<std::size_t>::quant,
		  "The type is not indexable");

  private:
    typedef typename boost::uint_t<value_traits<V>::quant + 1>::type enc;

  public:
    typedef Index<enc, std::less<enc> > index_type;
    typedef V value_type;
    static const bool inversible = true;

    index_type operator() (value_type x) const { return x - value_traits<V>::min() ; }
    value_type inv (index_type i) const { return i + value_traits<V>::min(); }
  };

  template <typename V>
  struct indexer<V, std::greater<V>, typename std::enable_if<std::is_integral<V>::value and
							     std::is_signed<V>::value>::type>
  {
  private:
    typedef typename boost::uint_t<value_traits<V>::quant + 1>::type enc;

  public:
    typedef Index<enc, std::greater<enc> > index_type;
    typedef V value_type;
    static const bool inversible = true;

    index_type operator() (value_type x) const { return x - value_traits<V>::min() ; }
    value_type inv (index_type i) const { return i + value_traits<V>::min(); }
  };

}

#endif // !MLN_CORE_VALUE_INDEXER_HPP
