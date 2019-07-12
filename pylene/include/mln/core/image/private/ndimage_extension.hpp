#pragma once
#include <type_traits>
#include <mln/core/point.hpp>


namespace mln::internal
{
  template <typename T, unsigned dim>
  struct ndimage_extension
  {
  private:
    typedef point<short, dim> P;

  public:
    typedef T               value_type;
    typedef std::true_type  support_fill;
    typedef std::true_type  support_mirror;
    typedef std::false_type support_periodize;

    ndimage_extension(char* ptr, const std::size_t* strides, const P& shp, int border);
    void fill(const T& v);
    void mirror();

  private:
    template <unsigned d>
    typename std::enable_if<(d < dim)>::type _fill(char* ptr, const T& v);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _fill(char* ptr, const T& v);

    template <unsigned d>
    typename std::enable_if<(d < dim)>::type _fillall(char* ptr, const T& v);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _fillall(char* ptr, const T& v);

    template <unsigned d>
    typename std::enable_if<(d < dim)>::type _mirror(char* ptr);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _mirror(char* ptr);

    template <unsigned d>
    typename std::enable_if<(d < dim)>::type _copy_line(char* src, char* dest);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _copy_line(char* src, char* dest);

  private:
    size_t m_strides[dim];
    P      m_shp;
    char*  m_ptr;
    int    m_border;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T, unsigned dim>
  ndimage_extension<T, dim>::ndimage_extension(char* ptr, const std::size_t* strides, const P& shp, int border)
    : m_shp(shp)
    , m_border(border)
  {
    m_ptr = ptr;
    for (unsigned i = 0; i < dim; ++i)
    {
      m_strides[i] = strides[i];
      m_ptr -= m_strides[i] * border;
    }
  }

  template <typename T, unsigned dim>
  void ndimage_extension<T, dim>::fill(const T& v)
  {
    _fill<0>(m_ptr, v);
  }

  template <typename T, unsigned dim>
  void ndimage_extension<T, dim>::mirror()
  {
    _mirror<0>(m_ptr);
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_mirror(char* ptr)
  {
    char* pori = ptr + m_border * m_strides[d];
    char* p    = pori;
    for (int i = 0; i < m_shp[d]; ++i, p += m_strides[d])
      _mirror<d + 1>(p);

    char* src1  = pori;
    char* dest1 = pori - m_strides[d];
    char* dest2 = pori + m_shp[d] * m_strides[d];
    char* src2  = dest2 - m_strides[d];

    for (int i = 0; i < m_border; ++i)
    {
      _copy_line<d + 1>(src1, dest1);
      _copy_line<d + 1>(src2, dest2);
      src1 += m_strides[d];
      dest1 -= m_strides[d];
      src2 -= m_strides[d];
      dest2 += m_strides[d];
    }
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_mirror(char* ptr)
  {
    (void)ptr;
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_copy_line(char* src, char* dest)
  {
    for (int k = 0; k < m_shp[d] + 2 * m_border; ++k)
    {
      _copy_line<d + 1>(src, dest);
      src += m_strides[d];
      dest += m_strides[d];
    }
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_copy_line(char* src, char* dest)
  {
    *(reinterpret_cast<T*>(dest)) = *(reinterpret_cast<T*>(src));
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_fill(char* ptr, const T& v)
  {
    for (int i = 0; i < m_border; ++i, ptr += m_strides[d])
      _fillall<d + 1>(ptr, v);

    for (int i = 0; i < m_shp[d]; ++i, ptr += m_strides[d])
      _fill<d + 1>(ptr, v);

    for (int i = 0; i < m_border; ++i, ptr += m_strides[d])
      _fillall<d + 1>(ptr, v);
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_fill(char* ptr, const T& v)
  {
    (void)ptr;
    (void)v;
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_fillall(char* ptr, const T& v)
  {
    for (int i = 0; i < m_shp[d] + (2 * m_border); ++i, ptr += m_strides[d])
      _fillall<d + 1>(ptr, v);
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_fillall(char* ptr, const T& v)
  {
    *(reinterpret_cast<T*>(ptr)) = v;
  }


} // namespace mln::internal
