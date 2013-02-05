#ifndef MLN_LABELING_BLOBS_HPP
# define MLN_LABELING_BLOBS_HPP

# include <mln/core/image/image.hpp>
# include <vector>

namespace mln
{

  namespace labeling
  {

    template <typename I, typename Neighborhood, typename Label = unsigned>
    std::pair< mln_ch_value(I, Label), Label >
    blobs(const Image<I>& ima, Neighborhood nbh, Label lbl = Label());


    /******************************/
    /*** Implementation         ***/
    /******************************/

    template <typename I, typename Label>
    std::pair< mln_ch_value(I, Label), Label >
    blobs(const Image<I>& ima, Neighborhood nbh, Label lbl)
    {
      typedef mln_value(I) V;
      typedef mln_point(I) P;
      mln_ch_value(I, Label) out;
      resize(out, ima);

      std::vector<P> queue;
      queue.reserve(ima.domain().size());

      mln_foreach(P p, ima.domain())
	{
	  if (out(p) == UNSEEN)
	    {
	      V val = ima(p);
	      queue.push_back(p);
	      ++lbl;
	      while (not queue.empty())
		{
		  q = queue.pop_back();
		  out(q) = lbl;
		  mln_forall(n)
		  {
		    if (ima.domain().has(n) and out(n) == UNSEEN and
			ima(n) == val)

		  }
		}
	    }
	}

    }

  }

}

#endif // ! MLN_LABELING_BLOBS_HPP
