#ifndef MLN_CORE_IMAGE_NDIMAGE_PIXEL_HPP
# define MLN_CORE_IMAGE_NDIMAGE_PIXEL_HPP

# include <mln/core/image/ndimage.hpp>

namespace mln
{

  template <typename T, unsigned dim, typename I>
  struct ndimage_pixel : Pixel< ndimage_pixel<T, dim, I> >
  {
    typedef mln::point<short, dim>              site_type;
    typedef site_type				point_type;
    typedef typename std::remove_const<T>::type value_type;
    typedef T&                                  reference;
    typedef ptrdiff_t                           distance_type;
    typedef I                                   image_type;
    typedef typename I::size_type		size_type;

    ndimage_pixel() = default;
    ndimage_pixel(image_type* ima, char* ptr) : m_ima (ima), m_ptr(ptr) {}


    /// \brief Copy constructor.
    ndimage_pixel(const ndimage_pixel& pix) = default;

    /// \brief Interopability constructor from the pixel type to the const one.
    template <typename U, typename Other>
    ndimage_pixel(const ndimage_pixel<U, dim, Other>& pix,
                  std::enable_if_t< !std::is_same<U,T>::value && std::is_convertible<U*,T*>::value, std::nullptr_t> = nullptr);

    reference	        val() const;
    site_type           point()  const;
    site_type           site()  const;
    image_type&         image() const;
    size_type		index() const { return static_cast<size_type>(m_index); }


    template <typename, unsigned, typename> friend struct ndimage_pixel;
    template <typename, unsigned, typename> friend struct ndimage_base;

  private:
    friend struct internal::iterator_core_access;

    size_t&                     get_index() { return m_index; }
    size_t                      get_index() const { return m_index; }
    mln::point<int, dim>&	get_point() { return m_point; }
    mln::point<int, dim>        get_point() const { return m_point; }
    std::ptrdiff_t              get_offset() const { return m_offset; }
    std::ptrdiff_t&             get_offset() { return m_offset; }

    template <typename U, typename Other>
    typename std::enable_if< std::is_convertible<U*,T*>::value, bool>::type
    equal(const ndimage_pixel<U, dim, Other>& other) const
    {
      return this->m_ptr == other.m_ptr;
    }


  private:
    image_type*                 m_ima;
    char*                       m_ptr; // redoundance with m_ima->m_ptr but helps vectorization

    mln::point<int, dim>        m_point;
    std::ptrdiff_t              m_offset;
    size_t                      m_index;
  };

/******************************************/
/****          Implementation          ****/
/******************************************/


  template <typename T, unsigned dim, typename I>
  template <typename U, typename Other>
  ndimage_pixel<T, dim, I>::ndimage_pixel(const ndimage_pixel<U, dim, Other>& pix,
                                          std::enable_if_t<!std::is_same<U,T>::value && std::is_convertible<U*,T*>::value, std::nullptr_t>)
    :
    m_ima (pix.m_ima),
    m_ptr (pix.m_ptr),
    m_point (pix.m_point),
    m_offset (pix.m_offset),
    m_index (pix.m_index)
  {
  }

  template <typename T, unsigned dim, typename I>
  typename ndimage_pixel<T,dim,I>::reference
  ndimage_pixel<T,dim,I>::val() const
  {
    return *reinterpret_cast<T*>(m_ptr + m_offset);
  }

  template <typename T, unsigned dim, typename I>
  typename ndimage_pixel<T,dim,I>::site_type
  ndimage_pixel<T,dim,I>::point() const
  {
    return m_point;
  }

  template <typename T, unsigned dim, typename I>
  typename ndimage_pixel<T,dim,I>::site_type
  ndimage_pixel<T,dim,I>::site() const
  {
    return m_point;
  }

  template <typename T, unsigned dim, typename I>
  typename ndimage_pixel<T,dim,I>::image_type&
  ndimage_pixel<T,dim,I>::image() const
  {
    mln_precondition(m_ima != NULL);
    return *m_ima;
  }


}

#endif
