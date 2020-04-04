#pragma once

#include <type_traits>

namespace mln
{

  /// \brief Pixel type definition of nd-images
  template <class T, unsigned dim, class E>
  struct ndimage_pixel;

  /// Fwd declaration
  template <typename T, unsigned dim, typename E>
  struct ndimage_base;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace details
  {

    template <typename T, unsigned dim>
    struct ndimage_pixel_base;

    template <typename T>
    struct ndimage_pixel_base<T, 0>
    {
      using value_type    = std::remove_const_t<T>;
      using reference     = T&;
      using distance_type = std::ptrdiff_t;
      using size_type     = unsigned;
      using pointer_type  = T*;

      ndimage_pixel_base() = default;

      ndimage_pixel_base(T* ptr)
        : m_ptr(ptr)
      {
      }

      template <class U>
      ndimage_pixel_base(const ndimage_pixel_base<U, 0>& other)
        : m_ptr(other.m_ptr)
        , m_index(other.m_index)
      {
      }

      reference val() const { return m_ptr[m_index]; }
      size_type index() const { return static_cast<size_type>(m_index); }

    protected:
      template <typename, unsigned>
      friend struct ndimage_pixel_base;

      template <typename, unsigned, typename>
      friend struct mln::ndimage_base;

      std::nullptr_t get_offset() const { return nullptr; }
      int            get_index() const { return m_index; }
      int&           get_index() { return m_index; }

    private:
      T*  m_ptr;
      int m_index;
    };

    template <unsigned dim>
    struct ndimage_pixel_base<void, dim>
    {
      using site_type  = mln::point<short, dim>;
      using point_type = site_type;

      site_type point() const { return m_point; }
      site_type site() const { return m_point; }

    protected:
      template <typename, unsigned, typename>
      friend struct mln::ndimage_base;

      auto& get_point() { return m_point; }
      auto  get_point() const { return m_point; }

    private:
      mln::point<int, dim> m_point;
    };

    template <typename T, unsigned dim>
    struct ndimage_pixel_base : ndimage_pixel_base<void, dim>, ndimage_pixel_base<T, 0>
    {
      friend struct internal::iterator_core_access;

      ndimage_pixel_base() = default;
      ndimage_pixel_base(T* ptr)
        : ndimage_pixel_base<T, 0>(ptr)
      {
      }

      template <class U>
      ndimage_pixel_base(const ndimage_pixel_base<U, dim>& other)
        : ndimage_pixel_base<void, dim>(other)
        , ndimage_pixel_base<T, 0>(other)
      {
      }
    };
  } // namespace details

  template <class T, unsigned dim, class I>
  struct ndimage_pixel : details::ndimage_pixel_base<T, dim>, Pixel<ndimage_pixel<T, dim, I>>
  {
    typedef I image_type;

    ndimage_pixel() = default;

    ndimage_pixel(image_type* ima, T* ptr = nullptr)
      : details::ndimage_pixel_base<T, dim>(ptr)
      , m_ima(ima)
    {
    }

    /// \brief Copy / copy conversion constructor
    template <class U, class J, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
    ndimage_pixel(const ndimage_pixel<U, dim, J>& pix)
      : details::ndimage_pixel_base<T, dim>(static_cast<const details::ndimage_pixel_base<U, dim>&>(pix))
      , m_ima(pix.m_ima)
    {
    }

    image_type& image() const { return *m_ima; }

  private:
    template <class, unsigned, class>
    friend struct ndimage_pixel;

    template <class, unsigned, class>
    friend struct ndimage_base;

    image_type* m_ima;
  };

  template <class T, unsigned dim, class I1, class I2>
  bool operator==(const ndimage_pixel<T, dim, I1>& lhs, const ndimage_pixel<T, dim, I2>& rhs)
  {
    return lhs.val() == rhs.val() && lhs.point() == rhs.point();
  }

  template <class T, unsigned dim, class I1, class I2>
  bool operator!=(const ndimage_pixel<T, dim, I1>& lhs, const ndimage_pixel<T, dim, I2>& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace mln
