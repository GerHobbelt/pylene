#ifndef MLN_CORE_PIXEL_POINTER_PIXEL_HPP
# define MLN_CORE_PIXEL_POINTER_PIXEL_HPP

# include <mln/core/concept/pixel.hpp>
# include <mln/core/assert.hpp>

namespace mln
{

  /// \brief Define a pixel accessing value through
  /// a pointer
  ///
  /// \tparam Image must be a model of Raw image
  template <class Image>
  struct pointer_pixel;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  namespace internal
  {
    // Fwd declaration.
    struct iterator_core_access;
  }


  template <class Image>
  struct pointer_pixel : Pixel< pointer_pixel<Image> >
  {
    using point_type = mln_point(Image);
    using site_type  = mln_point(Image);
    using size_type  = typename Image::size_type;
    using image_type = Image;
    using reference  = mln_reference(Image);
    using value_type = std::remove_reference_t<reference>;


    pointer_pixel() : m_image(nullptr) {}

    pointer_pixel(Image& image, char* ptr, const point_type& point, size_type index)
      : m_image(&image), m_ptr(ptr), m_point(point), m_index(index)
    {
    }

    pointer_pixel(const pointer_pixel&) = default;

    // Allow conversion pixel -> const pixel
    template <class I2>
    pointer_pixel(const pointer_pixel<I2>& other,
                  std::enable_if_t<std::is_convertible<I2*,Image*>::value,void*> = nullptr)
      : m_image(other.m_image), m_ptr(other.m_ptr), m_point(other.m_point), m_index(other.m_index)
    {
    }

    image_type&         image() const { mln_precondition(m_image); return *m_image; }
    reference           val() const   { mln_precondition(m_image); return *reinterpret_cast<value_type*>(m_ptr); }
    site_type           point() const { mln_precondition(m_image); return m_point; }
    site_type           site() const { mln_precondition(m_image); return m_point; }
    size_type		index() const { mln_precondition(m_image); return m_index; }
    constexpr int       weight() const { return 1; }

  private:
    // Enable this pixel to be compliant as an internal structure used for iterating
    friend struct internal::iterator_core_access;

    char* &		get_value() { return m_ptr; }
    site_type&		get_point() { return m_point; }
    size_type&		get_index() { return m_index; }
    const char*         get_value() const { return m_ptr; }
    const site_type&    get_point() const { return m_point; }
    size_type		get_index() const { return m_index; }


  private:
    image_type*         m_image;
    char*               m_ptr;
    site_type           m_point;
    size_type		m_index;
  };


} // end of namespace mln

#endif //!MLN_CORE_PIXEL_POINTER_PIXEL_HPP
