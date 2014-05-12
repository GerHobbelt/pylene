#ifndef MLN_CORE_PIXEL_UTILITY_HPP
# define MLN_CORE_PIXEL_UTILITY_HPP

namespace mln
{

  template <typename I, typename Pixel>
  struct rebinded_pixel;

  template <typename I, typename PixelIterator>
  struct rebind_pixel_iterator;

  // Meta operators for pixels
  struct get_pixel_value;
  struct get_pixel_point;

  namespace internal
  {
    /// \brief Helper class to know if a function accepts pixels or values
    template <typename I, class UnaryFunction>
    struct use_pix_helper;


    typedef mln::get_pixel_value meta_get_pixel_value;
    typedef mln::get_pixel_point meta_get_pixel_point;
  }

  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {

    template <typename I, class UnaryFunction>
    struct use_pix_helper
    {
      template <typename U>
      static
      std::true_type foo(const U&, decltype( std::declval<UnaryFunction>() (std::declval<U>()) )* = NULL);

      static
      std::false_type foo(...);

      typedef decltype(foo(std::declval<mln_pixel(I)>())) type;
    };

  }




  template <typename I, typename Pixel>
  struct rebinded_pixel : mln::Pixel< rebinded_pixel<I, Pixel> >
  {
  private:
    typedef typename std::remove_reference<Pixel>::type  pixel_t;
  public:
    typedef typename pixel_t::value_type   value_type;
    typedef typename pixel_t::reference    reference;
    typedef typename pixel_t::point_type   point_type;
    typedef typename pixel_t::site_type    site_type;
    typedef I                     image_type;

    rebinded_pixel() = default;
    rebinded_pixel(const rebinded_pixel&) = default;

    rebinded_pixel(I& ima, const Pixel& pix)
      : m_ima(&ima), m_pix(pix)
    {
    }


    template <typename J, typename Pixel2>
    rebinded_pixel(const rebinded_pixel<J, Pixel2>& other,
                   typename std::enable_if<std::is_convertible<J*, I*>::value and
                   std::is_convertible<Pixel2, Pixel>::value>::type* = NULL)
      : m_ima(other.m_ima),
	m_pix(other.m_pix)
    {
    }


    reference   val() const
    {
      return m_pix.val();
    }

    point_type  point() const
    {
      return m_pix.point();
    }

    point_type  site() const
    {
      return m_pix.site();
    }

    I& image() const
    {
      return *m_ima;
    }

  private:
    template <typename, typename>
    friend struct rebinded_pixel;

    template <typename, typename>
    friend struct rebind_pixel_iterator;


    I*    m_ima;
    Pixel m_pix;
  };


  template <typename I, typename PixelIterator>
  struct rebind_pixel_iterator :
    iterator_base <rebind_pixel_iterator<I, PixelIterator>,
                   rebinded_pixel<I, typename PixelIterator::value_type>,
                   rebinded_pixel<I, typename PixelIterator::reference> >
  {
  private:
    static const bool use_ref = std::is_reference<typename PixelIterator::reference>::value;
    typedef rebinded_pixel<I, typename PixelIterator::reference> pixel_t;

  public:
    rebind_pixel_iterator() = default;

    rebind_pixel_iterator(const rebind_pixel_iterator& other) = default;

    rebind_pixel_iterator(I& ima, const PixelIterator& pixter)
      : m_ima(&ima),
	m_pixter(pixter)
    {
    }

    template <typename J, typename PixelIterator2>
    rebind_pixel_iterator(const rebind_pixel_iterator<J, PixelIterator2>& other,
                            typename std::enable_if< std::is_convertible<J*, I*>::value and
                            std::is_convertible<PixelIterator2, PixelIterator>::value>::type* = NULL)
      : m_ima(other.m_ima),
	m_pixter(other.m_pixter)
    {
    }

    void init()
    {
      m_pixter.init();
    }

    void next()
    {
      m_pixter.next();
    }

    pixel_t dereference() const
    {
      return pixel_t(*m_ima, *m_pixter);
    }

    bool finished() const
    {
      return m_pixter.finished();
    }


  private:
    template <typename, typename>
    friend struct rebind_pixel_iterator;
    I*			m_ima;
    PixelIterator       m_pixter;
  };


  struct get_pixel_value
  {
    template <typename Pix>
    struct apply { typedef typename Pix::reference type; };

    template <typename Pix>
    typename Pix::reference
    operator() (const Pix& pix) const
    {
      return pix.val();
    }
  };

  struct get_pixel_point
  {
    template <typename Pix>
    struct apply { typedef typename Pix::point_type type; };

    template <typename Pix>
    typename Pix::point_type
    operator() (const Pix& pix) const
    {
      return pix.point();
    }
  };



} // end of namespace mln

#endif //!MLN_CORE_PIXEL_UTILITY_HPP
