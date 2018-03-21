#pragma once

#include "ndimage.hpp"

namespace mln
{

  template <int N, class V, layout_t L>
  class ndimage;

  namespace details
  {


    /// \brief Pixel type definition of nd-images
    template <int N, class V, class E>
    struct ndimage_pixel;

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <typename T, int N>
    struct ndimage_pixel_base;

    template <typename T>
    struct ndimage_pixel_base<T, 0>
    {
      using value_type = std::remove_const_t<T>;
      using reference = T&;
      using index_type = std::ptrdiff_t;
      using size_type [[deprecated]] = index_type;
      using pointer_type = T*;

      ndimage_pixel_base() = default;

      ndimage_pixel_base(T* ptr) : m_ptr(ptr) {}

      template <class U>
      ndimage_pixel_base(const ndimage_pixel_base<U, 0>& other) : m_ptr(other.m_ptr), m_index(other.m_index)
      {
      }

      reference  val() const { return m_ptr[m_index]; }
      index_type index() const { return m_index; }

    protected:
      template <typename, int>
      friend struct ndimage_pixel_base;

      template <int M, class V, layout_t L>
      friend class mln::ndimage;

      std::nullptr_t get_offset() const { return nullptr; }
      std::ptrdiff_t get_index() const { return m_index; }
      std::ptrdiff_t& get_index() { return m_index; }

    private:
      T* m_ptr;
      std::ptrdiff_t m_index;
    };

    template <int N>
    struct ndimage_pixel_base<void, N>
    {
      using point_type = mln::point<std::ptrdiff_t, N>;
      using site_type [[deprecated]] = point_type;

      site_type point() const { return m_point; }
      site_type site() const { return m_point; }

    protected:

      auto& get_point() { return m_point; }
      auto get_point() const { return m_point; }

      template <int M, class V, layout_t L>
      friend class mln::ndimage;

    private:
      point_type m_point;
    };

    template <typename T, int N>
    struct ndimage_pixel_base : ndimage_pixel_base<void, N>, ndimage_pixel_base<T, 0>
    {
      friend struct internal::iterator_core_access;

      ndimage_pixel_base() = default;
      ndimage_pixel_base(T* ptr) : ndimage_pixel_base<T, 0>(ptr) {}

      template <class U>
      ndimage_pixel_base(const ndimage_pixel_base<U, N>& other)
          : ndimage_pixel_base<void, N>(other), ndimage_pixel_base<T, 0>(other)
      {
      }
    };

    template <int N, class V, class E>
    struct ndimage_pixel : ndimage_pixel_base<V, N>, Pixel<ndimage_pixel<N, V, E>>
    {
      using image_type = E;

      ndimage_pixel() = default;

      ndimage_pixel(image_type* ima, V* ptr = nullptr) : details::ndimage_pixel_base<V, N>(ptr), m_ima(ima) {}

      /// \brief Copy / copy conversion constructor
      template <class U, class J, typename = std::enable_if_t<std::is_convertible<U*, V*>::value>>
      ndimage_pixel(const ndimage_pixel<N, U, J>& pix)
          : ndimage_pixel_base<V, N>(static_cast<const ndimage_pixel_base<U, N>&>(pix)), m_ima(pix.m_ima)
      {
      }

      image_type& image() const { return *m_ima; }

    private:
      template <int, class, class>
      friend struct ndimage_pixel;

      image_type* m_ima;
    };

  }
}
