#ifndef MLN_CORE_EXTENSION_EXTENSION_HPP
# define MLN_CORE_EXTENSION_EXTENSION_HPP

# include <mln/core/extension/extension_traits.hpp>
# include <mln/core/image/image.hpp>
# include <mln/core/image/morphers/extended_by_value_image.hpp>

namespace mln
{

  namespace extension
  {



/******************************************/
/****          Free functions          ****/
/******************************************/


  /// \brief Remove the extension of an image.
  ///
  /// \p remove_extension recursively removes the extensions of an image until
  /// getting an image without extension or an image whose extension cannot be
  /// removed. This function has to be overloaded by the morphers extending an
  /// image. This default implementation returns the input image as such.
  ///
  /// \param ima input image
  ///
  /// \return The image without extension
  ///
  template <typename I>
  const I&
  remove_extension(const Image<I>& ima);

  template <typename I>
  I&
  remove_extension(Image<I>& ima);

  template <typename I>
  I&&
  remove_extension(Image<I>&& ima);


  /// \brief Add an infinite value extension to the image. In the resulting
  /// image, every access outside the image domain yields in the extension value.
  ///
  /// \param ima input image
  /// \param value_extension_tag
  ///
  /// \return an image extended by value
  ///
  template <typename I>
  extended_by_value_image<const I&>
  add_value_extension(const Image<I>& ima, const mln_value(I)& v);

  template <typename I>
  extended_by_value_image<I&>
  add_value_extension(Image<I>& ima, const mln_value(I)& v);

  template <typename I>
  extended_by_value_image<I>
  add_value_extension(Image<I>&& ima, const mln_value(I)& v);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <typename I>
  const I&
  remove_extension(const Image<I>& ima)
  {
    return exact(ima);
  }

  template <typename I>
  I&
  remove_extension(Image<I>& ima)
  {
    return exact(ima);
  }

  template <typename I>
  I&&
  remove_extension(Image<I>&& ima)
  {
    return move_exact(ima);
  }

  template <typename I>
  extended_by_value_image<const I&>
  add_value_extension(const Image<I>& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<const I&>(exact(ima), v);
  }

  template <typename I>
  extended_by_value_image<I&>
  add_value_extension(Image<I>& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<I&>(exact(ima), v);
  }

  template <typename I>
  extended_by_value_image<I>
  add_value_extension(Image<I>&& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<I>(move_exact(ima), v);
  }




} // end of namespace mln::extension
} // end of namespace mln

#endif //!MLN_CORE_EXTENSION_EXTENSION_HPP
