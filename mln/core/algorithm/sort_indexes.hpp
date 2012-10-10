#ifndef MLN_CORE_ALGORITHM_SORT_INDEXES_HPP
# define MLN_CORE_ALGORITHM_SORT_INDEXES_HPP

# include <mln/core/value/value_traits.hpp>
# include <mln/core/image/image.hpp>
# include <vector>

// FIXME: use indexer

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
    template <typename I>
    std::vector<typename I::size_type>
    sort_indexes(const I& input, std::less<mln_value(I)>, std::true_type _is_low_quant_)
    {
      (void) _is_low_quant_;

      typedef mln_value(I) V;
      static constexpr V sup = value_traits<V>::sup();
      unsigned h[sup+1] = {0,};
      {
	mln_viter(v, input);
	mln_forall(v)
	  ++h[*v];

	unsigned count = 0;
	for (int i = 0; i < sup+1; ++i)
	  {
	    unsigned tmp = h[i];
	    h[i] = count;
	    count += tmp;
	  }
      }

      std::vector<typename I::size_type> v;
      v.resize(input.domain().size());
      {
	mln_pixter(px, input);
	mln_forall(px)
	  v[ h[px->val()]++ ] = px->index();
      }
      return v;
    }

  } // end of namespace mln::impl


  template <typename I, typename BinaryFunction>
  std::vector<typename I::size_type>
  sort_indexes(const Image<I>& input,  BinaryFunction cmp)
  {
    static_assert(std::is_same<typename image_category<I>::type, raw_image_tag>::value,
		  "Image must model the Raw Image Concept");
    typedef std::integral_constant<bool, (value_traits<mln_value(I)>::quant <= 16)> is_low_quant;
    return impl::sort_indexes(exact(input), cmp, is_low_quant ());
  }

} // end of namespace mln


#endif // !MLN_CORE_ALGORITHM_SORT_INDEXES_HPP
