#ifndef EXTENDED_BY_VALUE_IMAGE_HPP
# define EXTENDED_BY_VALUE_IMAGE_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/morphers/morpher_base.hpp>

namespace mln
{


  /// \brief An image with an extension defined by a value
  template <typename I>
  struct extended_by_value_image;

  /// \{

  /// \brief Add an extension by value to an image
  template <typename I>
  extended_by_value_image<const I&>
  extend_by_value(const Image<I>& ima, const mln_value(I)& v);

  template <typename I>
  extended_by_value_image<I&>
  extend_by_value(Image<I>& ima, const mln_value(I)& v);

  template <typename I>
  extended_by_value_image<I&>
  extend_by_value(Image<I>&& ima, const mln_value(I)& v);
  /// \}



  template <typename V>
  struct by_value_extension;


  /*****************************/
  /***  Traits               ***/
  /*****************************/

  template <typename I>
  struct image_traits< extended_by_value_image<I> >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    // Actually if I is concrete we could set this image concrete as well
    // but since adding this extension prevents indexation we better use
    // the concrete image of I rather than this morpher, as long as concretization
    // does not impose any property about this extension.

    typedef std::false_type				concrete;
    typedef typename image_traits<image_t>::category	category;
    typedef typename image_traits<image_t>::accessible	accessible;

    // we can check if an index belongs to a domain using ima.domain().has(ima.point_at_index(i))
    // but this is not efficient. We better use points directly.
    typedef std::false_type				indexable;
    typedef std::true_type				has_extension;
  };


  /*****************************/
  /**** Implementation       ***/
  /*****************************/


  template <typename I>
  struct extended_by_value_image : morpher_base< extended_by_value_image<I>, I,
					       typename image_point<typename std::remove_reference<I>::type>::type,
					       typename image_value<typename std::remove_reference<I>::type>::type >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    typedef image_t::point_type			point_type;
    typedef image_t::domain_type		domain_type;
    typedef image_t::value_type			value_type;
    typedef typename image_reference<image_t>::type		reference;
    typedef typename image_const_reference<image_t>::type	const_reference;
    typedef typename image_pixel<image_t>::type			pixel_type;
    typedef typename image_const_pixel<image_t>::type		const_pixel_type;

    typedef typename image_value_range<image_t>::type		value_range;
    typedef typename image_const_value_range<image_t>::type	const_value_range;
    typedef typename image_pixel_range<image_t>::type		pixel_range;
    typedef typename image_const_pixel_range<image_t>::type	const_pixel_range;

    typedef by_value_extension<value_type>	extension_type;


    extended_by_value_image(I&& ima, const value_type& val)
      : m_ima(std::forward<I>(ima)), m_val (val)
    {
    }

    template <typename = void>
    typename std::enable_if<image_traits<image_t>::accessible::value, reference>::type
    at (const point_type& p)
    {
      if (m_ima.domain().has(p))
	return m_ima(p);
      else
	return m_val;
    }

    template <typename = void>
    typename std::enable_if<image_traits<image_t>::accessible::value, const_reference>::type
    at (const point_type& p) const
    {
      if (m_ima.domain().has(p))
	return m_ima(p);
      else
	return m_val;
    }

    extension_type extension()
    {
      return extension_type (m_val);
    }


  private:
    I		 m_ima;
    mln_value(I) m_val;
  };

  template <typename V>
  struct by_value_extension
  {
    by_value_extension(V& val)
    : m_val(val)
    {
    }

    void fill(const V& val)
    {
      m_val = val;
    }

  private:
    V& m_val;
  };


  template <typename I>
  extended_by_value_image<const I&>
  extend_by_value(const Image<I>& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<const I&>(exact(ima), v);
  }

  template <typename I>
  extended_by_value_image<I&>
  extend_by_value(Image<I>& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<const I&>(exact(ima), v);
  }

  template <typename I>
  extended_by_value_image<I>
  extend_by_value(Image<I>&& ima, const mln_value(I)& v)
  {
    return extended_by_value_image<I>(move_exact(ima), v);
  }


}


#endif // ! EXTENDED_BY_VALUE_IMAGE_HPP
