#ifndef MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
# define MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP

# include <mln/core/algorithm/clone.hpp>

namespace mln
{

  /// \defgroup free_functions Free functions
  /// \ingroup image
  /// \{


  /// \fn void reindex(Image<I>& ima1, const Image<J>& ima2);
  /// \brief Reindex \p ima1 to match indexes of ima2.
  ///
  /// It ensures that after reindexation, \p ima1.domain().pmin and \p ima2.domain().pmin
  /// have the same index. If \p ima1 and/or \p ima2 are not indexable, the function has no effect.
  ///
  /// Reindexation can be usefull when using view e.g. subimage and still be able to use indexes.
  ///
  /// \param ima1 The image to be reindexed.
  /// \param ima2 The reference image.
  ///
  ///
  template <typename I, typename J>
  typename std::enable_if<image_traits<I>::indexable::value and image_traits<J>::indexable::value>::type
  reindex(Image<I>& ima1, const Image<J>& ima2);


  /// \brief Initialize an image from an other such that
  /// domains correspond. If \p ima and \p other are indexable
  /// indexes match as well.
  ///
  /// \param ima Image to be initialized.
  /// \param other Image to initialize with.
  /// \param border Require a border of custom size.
  /// \param v Initialize memory with a custom value.
  ///
  template <typename I, typename J>
  void resize(Image<I>& ima, const Image<J>& other, int border, mln_value(I) v);

  /// \overload
  template <typename I, typename J>
  void resize(Image<I>& ima, const Image<J>& other);


  /// \brief Return a concrete version of the image.
  ///
  /// \p eval returns a concrete version of \p ima. If
  /// If \p ima is already concrete then it returns the
  /// image unmodified. Otherwise, it clones the content of
  /// \p ima.
  ///
  /// \param ima Image to set concrete
  ///
  /// \return A concrete duplicate of the image or the image itself.
  ///
  template <typename InputImage>
  typename std::enable_if< image_traits<InputImage>::concrete::value, InputImage&& >::type
  eval(InputImage&& ima);

  ///\}


  /*************************/
  /***  Implementation   ***/
  /*************************/


  template <typename I, typename J>
  inline
  typename std::enable_if<image_traits<I>::indexable::value and image_traits<J>::indexable::value>::type
  reindex(Image<I>& ima, const Image<J>& ima2)
  {
    exact(ima).reindex(exact(ima2).index_of_point(exact(ima2).domain().pmin));
  }

  template <typename I, typename J>
  inline
  typename std::enable_if<!(image_traits<I>::indexable::value and image_traits<J>::indexable::value)>::type
  reindex(Image<I>&, const Image<J>&)
  {
  }

  template <typename I, typename J>
  inline
  void resize(Image<I>& ima, const Image<J>& other)
  {
    exact(ima).resize(exact(other).domain());
    reindex(ima, other);
  }


  template <typename I, typename J>
  inline
  void resize(Image<I>& ima, const Image<J>& other, int border, mln_value(I) v)
  {
    exact(ima).resize(exact(other).domain(), border, v);
    reindex(ima, other);
  }


  template <typename InputImage>
  inline
  typename std::enable_if< image_traits<InputImage>::concrete::value, InputImage&& >::type
  eval(InputImage&& ima)
  {
    return std::forward<InputImage>(ima);
  }

  template <typename InputImage>
  inline
  typename std::enable_if< !image_traits<InputImage>::concrete::value,
			   mln_concrete(typename std::remove_reference<InputImage>::type)>::type
  eval(InputImage&& ima)
  {
    return clone(std::forward<InputImage>(ima));
  }


}

#endif // !MLN_CORE_IMAGE_IMAGE_ROUTINES_HPP
