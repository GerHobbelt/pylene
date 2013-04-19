#ifndef EXTENDED_BY_VALUE_IMAGE_HPP
# define EXTENDED_BY_VALUE_IMAGE_HPP

# include <mln/core/image/image.hpp>

namespace mln
{


  /// \brief An image with an extension defined by a value
  template <typename I>
  struct extended_by_value_image;

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
    typedef typename image_traits<image_t>::category	category;
    typedef typename image_traits<image_t>::accessible	accessible;
    typedef typename image_traits<image_t>::indexable   indexable;
    typedef std::false_type				concrete;
    typedef std::true_type				has_extension;
  };


  /*****************************/
  /**** Implementation       ***/
  /*****************************/


  template <typename I>
  struct extended_by_value_image : image_base< extended_by_value_image<I>,
					       typename image_point<typename std::remove_reference<I>::type>::type,
					       typename image_value<typename std::remove_reference<I>::type>::type >
  {
  private:
    typedef typename std::remove_reference<I>::type image_t;

  public:
    typedef image_t::point_type			point_type;
    typedef image_t::domain_type		domain_type;
    typedef image_t::value_type			value_type;
    typedef image_t::reference			reference;
    typedef image_t::const_reference		const_reference;
    typedef image_t::pixel_type			pixel_type;
    typedef image_t::const_pixel_type		const_pixel_type;

    typedef image_t::value_range		value_range;
    typedef image_t::const_value_range		const_value_range;
    typedef image_t::pixel_range		pixel_range;
    typedef image_t::const_pixel_range		const_pixel_range;

    typedef by_value_extension<value_type>	extension_type;


    extended_by_value_image(I&& ima, const value_type& val)
      : m_ima(std::forward<I>(ima)), m_val (val)
    {
    }

    template <typename = void>
    typename std::enable_if<image_traits<image_t>::accessible::value, reference>::type
    operator() (const point_type& p)
    {
      if (m_ima.domain().has(p))
	return m_ima(p);
      else
	return m_val;
    }

    template <typename = void>
    typename std::enable_if<image_traits<image_t>::accessible::value, const_reference>::type
    operator() (const point_type& p) const
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
  }

}


#endif // ! EXTENDED_BY_VALUE_IMAGE_HPP
