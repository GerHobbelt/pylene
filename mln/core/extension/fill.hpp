#ifndef MLN_CORE_EXTENSION_FILL_HPP
# define MLN_CORE_EXTENSION_FILL_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/domain/box.hpp>
# include <algorithm>

namespace mln
{
  namespace extension
  {

    template <typename I>
    I& fill(Image<I>& ima, mln_value(I) v);

    template <typename I>
    I&& fill(Image<I>&& ima, mln_value(I) v);


    /*******************/
    /** Implementation */
    /*******************/

    namespace impl
    {

      template <typename I>
      void fill(I& ima, mln_value(I) v, box2d, std::true_type __is_raw_image__)
      {
	(void) __is_raw_image__;
	typedef mln_value(I) V;

	int nrows = ima.nrows();
	int border = ima.border();
	int nbcols = ima.ncols() + 2 * border;

	// Fill border rows
	if (sizeof(V) == ima.strides()[1])
	  {
	    char* ptr = (char*)&ima.at(ima.domain().pmin - border);
	    for (int j = 0; j < border; ++j)
	      {
		std::fill((V*)ptr, (V*)ptr + nbcols, v);
		ptr += ima.strides()[0];
	      }
	    ptr += ima.strides()[0] * nrows;
	    for (int j = 0; j < border; ++j)
	      {
		std::fill((V*)ptr, (V*)ptr + nbcols, v);
		ptr += ima.strides()[0];
	      }
	  }
	else
	  {
	    char* ptr = (char*)&ima[0];
	    for (int j = 0; j < border; ++j)
	      {
		for (int i = 0; i < nbcols; ++i)
		  *(V*)(ptr + i * ima.strides()[1]) = v;
		ptr += ima.strides()[0];
	      }
	    ptr += ima.strides()[0] * nrows;
	    for (int j = 0; j < border; ++j)
	      {
		for (int i = 0; i < nbcols; ++i)
		  *(V*)(ptr + i * ima.strides()[1]) = v;
		ptr += ima.strides()[0];
	      }
	  }

	// Fill border cols
        mln_point(I) p = ima.domain().pmin;
	for (int i = 0; i < nrows; ++i, ++p[0])
	  {
	    for (p[1] = ima.domain().pmin[1]-border; p[1] < ima.domain().pmin[1]; ++p[1])
	      ima.at(p) = v;
	    for (p[1] = ima.domain().pmax[1]; p[1] < ima.domain().pmax[1]+border; ++p[1])
	      ima.at(p) = v;
	  }
      }

    } // end of namespace mln::extension::impl


    template <typename I>
    I& fill(Image<I>& ima, mln_value(I) v)
    {
      impl::fill(exact(ima), v, exact(ima).domain(),
                 std::is_same<typename image_traits<I>::category, raw_image_tag> ());
      return exact(ima);
    }


    template <typename I>
    I&& fill(Image<I>&& ima, mln_value(I) v)
    {
      fill(exact(ima), v);
      return move_exact(ima);
    }


  } // end of namespace mln::extension
} // end of namespace mln


#endif // ! MLN_CORE_EXTENSION_FILL_HPP
