#ifndef MLN_CORE_ALGORITHM_SORT_INDEXES_HPP
# define MLN_CORE_ALGORITHM_SORT_INDEXES_HPP

# include <mln/core/value/value_traits.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/value/indexer.hpp>
# include <vector>
# include <algorithm>

// FIXME: Speed up HQ version

namespace mln
{

  template <typename I, typename BinaryFunction = std::less<mln_value(I)> >
  std::vector<typename I::size_type>
  sort_indexes(const Image<I>& input,  BinaryFunction cmp = BinaryFunction ());


  /****************/
  /* Implem       */
  /****************/

  namespace impl
  {
    template <typename I, typename StrictWeakOrdering,
	      typename Indexer = indexer<mln_value(I), StrictWeakOrdering> >
    std::vector<typename I::size_type>
    sort_indexes(const I& input, StrictWeakOrdering, std::true_type _is_low_quant_)
    {
      (void) _is_low_quant_;
      typedef typename Indexer::index_type index_t;
      Indexer f;


      typedef mln_value(I) V;
      static constexpr std::size_t nvalues = 1 << value_traits<index_t>::quant;
      unsigned h[nvalues] = {0,};
      {
	mln_viter(v, input);
	mln_forall(v)
	  ++h[f(*v)];

	unsigned count = 0;
        index_t i = value_traits<index_t>::min();
	do {
          unsigned tmp = h[i];
          h[i] = count;
          count += tmp;
        } while (i++ < value_traits<index_t>::max());
        assert(count == input.domain().size());
      }

      std::vector<typename I::size_type> v;
      v.resize(input.domain().size());
      {
	mln_pixter(px, input);
	mln_forall(px)
	  v[ h[f(px->val())]++ ] = px->index();
      }
      return v;
    }

    template <typename I, typename StrictWeakOrdering,
	      typename Indexer = indexer<mln_value(I), StrictWeakOrdering> >
    std::vector<typename I::size_type>
    sort_indexes(const I& input, StrictWeakOrdering cmp, std::false_type _is_low_quant_)
    {
      (void) _is_low_quant_;
      typedef typename I::size_type size_type;
      std::vector<size_type> v;
      v.reserve(input.domain().size());
      mln_pixter(px, input);
      mln_forall(px)
	v.push_back(px->index());

      std::sort(v.begin(), v.end(), [&input, cmp](size_type x, size_type y) { return cmp(input[x], input[y]); });
      return v;
    }


  } // end of namespace mln::impl


  template <typename I, typename BinaryFunction>
  std::vector<typename I::size_type>
  sort_indexes(const Image<I>& input, BinaryFunction cmp)
  {
    static_assert(std::is_same<typename image_category<I>::type, raw_image_tag>::value,
		  "Image must model the Raw Image Concept");
    typedef std::integral_constant<bool, (value_traits<mln_value(I)>::quant <= 16)> is_low_quant;
    return impl::sort_indexes(exact(input), cmp, is_low_quant ());
  }

} // end of namespace mln


#endif // !MLN_CORE_ALGORITHM_SORT_INDEXES_HPP
