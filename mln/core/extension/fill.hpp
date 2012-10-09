#ifndef MLN_CORE_EXTENSION_FILL_HPP
# define MLN_CORE_EXTENSION_FILL_HPP

# include <mln/core/image/image.hpp>
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
      I& fill(I& ima, mln_value(I) v, std::true_type __is_raw_image_and_box2d__)
      {
	(void) __is_raw_image_and_box__;
	typedef mln_value(I) V;

	int ncols = ima.ncols();
	int nrows = ima.nrows();
	int border = ima.border();
	int nbcols = ima.ncols() + 2 * border;
	int nbrows = ima.nrows() + 2 * border;

	// Fill border rows
	if (sizeof(V) == strides_[1])
	  {
	    char* ptr = &ima[0];
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
	    char* ptr = &ima[0];
	    for (int j = 0; j < border; ++j)
	      {
		for (int i = 0; i < nbcols; ++i)
		  *(V*)(ptr + i * strides()[1]) = v;
		ptr += ima.strides()[0];
	      }
	    ptr += ima.strides()[0] * nrows;
	    for (int j = 0; j < border; ++j)
	      {
		for (int i = 0; i < nbcols; ++i)
		  *(V*)(ptr + i * strides()[1]) = v;
		ptr += ima.strides()[0];
	      }
	  }

	// Fill border cols
	for (int i = 0; i < nrows; ++i)
	  {
	    for (int j = -3; j < 0; ++j)
	      ima.at_(point2d{i,j}) = v;
	    for (int j = ncols; j < ncols+3; ++j)
	      ima.at_(point2d{i,j}) = v;
	  }
      }


    }


  }

}

#endif // ! MLN_CORE_EXTENSION_FILL_HPP
