#ifndef MLN_LABELING_BLOBS_HPP
# define MLN_LABELING_BLOBS_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/value/value_traits.hpp>
//# include <mln/core/algorithm/fill.hpp>
# include <mln/core/extension/fill.hpp>
# include <vector>
# include <type_traits>

namespace mln
{

  namespace labeling
  {

    /// \brief labelize connected components of a binary image ima.
    template <typename I, typename Neighborhood, typename Label = unsigned>
    std::pair< mln_ch_value(I, Label), Label >
    blobs(const Image<I>& ima, Neighborhood nbh, Label lbl = Label());


    /******************************/
    /*** Implementation         ***/
    /******************************/

    template <typename I, typename Neighborhood, typename Label>
    std::pair< mln_ch_value(I, Label), Label >
    blobs(const Image<I>& ima_, Neighborhood nbh, Label lbl)
    {
      typedef mln_value(I) V;
      typedef mln_point(I) P;
      static_assert(std::is_same<V, bool>::value, "Only supports binary image (type: bool)");


      const I& ima = exact(ima_);
      Label bg = lbl;

      mln_ch_value(I, Label) out;
      bool nocheck_boundary = resize(out, ima).adjust(nbh).init(bg);

      std::vector<P> queue;
      queue.reserve(ima.domain().size());

      if (nocheck_boundary)
	{
	  extension::fill(out, value_traits<Label>::max());
	  P q;
	  mln_iter(n, nbh(q));

	  mln_foreach(P p, ima.domain())
	    {
	      if (ima(p) and out(p) == bg)
		{
		  queue.push_back(p);
		  ++lbl;
		  mln_assertion(lbl <= value_traits<Label>::max());
		  while (not queue.empty())
		    {
		      q = queue.back();
		      queue.pop_back();
		      out(q) = lbl;
		      mln_forall(n)
			if (out.at(*n) == bg and ima(*n))
			  queue.push_back(*n);
		    }
		}
	    }
	}
      else
	{
	  P q;
	  mln_iter(n, nbh(q));

	  mln_foreach(P p, ima.domain())
	    {
	      if (ima(p) and out(p) == bg)
		{
		  queue.push_back(p);
		  ++lbl;
		  mln_assertion(lbl <= value_traits<Label>::max());
		  while (not queue.empty())
		    {
		      q = queue.back();
		      queue.pop_back();
		      out(q) = lbl;
		      mln_forall(n)
			if (ima.domain().has(*n) and ima(*n) and out(*n) == bg)
			  queue.push_back(*n);
		    }
		}
	    }
	}

      return std::make_pair(out, lbl);
    }

  }

}

#endif // ! MLN_LABELING_BLOBS_HPP
