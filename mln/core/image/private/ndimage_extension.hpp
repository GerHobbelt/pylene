#pragma once

#include <mln/core/domain/box.hpp>

namespace mln
{

  namespace details
  {

    template <int N, typename V>
    struct ndimage_extension
    {
    private:
      using domain_type = mln::box<int, N>;

    public:
      typedef V value_type;
      typedef std::true_type support_fill;
      typedef std::true_type support_mirror;
      typedef std::false_type support_periodize;

      ndimage_extension(const domain_type& domain,
                        const domain_type& vdomain,
                        const std::ptrdiff_t strides[],
                        V* ptr);

      void fill(const V& v);
      void mirror();

    private:
      template <int d>
      std::enable_if_t<(d < N)> _fill(V* ptr, const V& v);

      template <int d>
      std::enable_if_t<(d == N)> _fill(V* ptr, const V& v);

      template <int d>
      std::enable_if_t<(d < N)> _fillall(V* ptr, const V& v);

      template <int d>
      std::enable_if_t<(d == N)> _fillall(V* ptr, const V& v);

      template <int d>
      std::enable_if_t<(d < N)> _mirror(V* ptr);

      template <int d>
      std::enable_if_t<(d == N)> _mirror(V* ptr);

      template <int d>
      std::enable_if_t<(d < N)> _copy_line(V* src, V* dest);

      template <int d>
      std::enable_if_t<(d == N)> _copy_line(V* src, V* dest);

    private:
      domain_type    m_virtual_domain;
      domain_type    m_domain;
      std::ptrdiff_t m_strides[N];
      V*             m_ptr;
    };


    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <int N, typename V>
    ndimage_extension<N, V>::ndimage_extension(const domain_type& domain,
                                                 const domain_type& vdomain,
                                                 const std::ptrdiff_t strides[],
                                                 V* ptr)
      : m_virtual_domain(vdomain), m_domain(domain), m_ptr(ptr)
    {
      for (int k = 0; k < N; ++k)
        m_strides[k] = strides[k];
    }

    template <int N, typename V>
    void ndimage_extension<N, V>::fill(const V& v)
    {
      _fill<0>(m_ptr, v);
    }

    template <int N, typename V>
    void ndimage_extension<N, V>::mirror()
    {
      _mirror<0>(m_ptr);
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d < N)> ndimage_extension<N, V>::_mirror(V* const ptr)
    {
      V* psrc = ptr + (m_domain.pmin[d] - m_virtual_domain.pmin[d]) * m_strides[d];

      {
        V* p = psrc;
        for (int i = m_domain.pmin[d]; i < m_domain.pmax[d]; ++i)
        {
          _mirror<d + 1>(p);
          p += m_strides[d];
        }
      }


      // Mirror left
      {
        V* src = psrc;
        V* dst = psrc - m_strides[d];
        for (int i = m_virtual_domain.pmin[d]; i < m_domain.pmin[d]; ++i)
        {
          _copy_line<d+1>(src, dst);
          dst -= m_strides[d];
          src += m_strides[d];
        }
      }

      // Mirror Right
      {
        V* dst = ptr + (m_domain.pmax[d] - m_virtual_domain.pmin[d]) * m_strides[d];
        V* src = dst - m_strides[d];
        for (int i = m_virtual_domain.pmin[d]; i < m_domain.pmin[d]; ++i)
        {
          _copy_line<d+1>(src, dst);
          dst += m_strides[d];
          src -= m_strides[d];
        }
      }
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d == N)> ndimage_extension<N, V>::_mirror(V* ptr)
    {
      (void)ptr;
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d < N)> ndimage_extension<N, V>::_copy_line(V* src, V* dest)
    {
      for (int i = m_virtual_domain.pmin[d]; i < m_virtual_domain.pmax[d]; ++i)
      {
        _copy_line<d + 1>(src, dest);
        src += m_strides[d];
        dest += m_strides[d];
      }
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d == N)> ndimage_extension<N, V>::_copy_line(V* src, V* dest)
    {
      *dest = *src;
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d < N)> ndimage_extension<N, V>::_fill(V* ptr, const V& v)
    {
      for (int i = m_virtual_domain.pmin[d]; i < m_domain.pmin[d]; ++i)
      {
        _fillall<d + 1>(ptr, v);
        ptr += m_strides[d];
      }

      for (int i = m_domain.pmin[d]; i < m_domain.pmax[d]; ++i)
      {
        _fill<d + 1>(ptr, v);
        ptr += m_strides[d];
      }

      for (int i = m_domain.pmax[d]; i < m_virtual_domain.pmax[d]; ++i)
      {
        _fillall<d + 1>(ptr, v);
        ptr += m_strides[d];
      }
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d == N)> ndimage_extension<N, V>::_fill(V* ptr, const V& v)
    {
      (void)ptr;
      (void)v;
    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d < N)> ndimage_extension<N, V>::_fillall(V* ptr, const V& v)
    {
      for (int i = m_virtual_domain.pmin[d]; i < m_virtual_domain.pmax[d]; ++i)
      {
        _fillall<d + 1>(ptr, v);
        ptr += m_strides[d];
      }

    }

    template <int N, typename V>
    template <int d>
    std::enable_if_t<(d == N)> ndimage_extension<N, V>::_fillall(V* ptr, const V& v)
    {
      *ptr = v;
    }


  }

}
