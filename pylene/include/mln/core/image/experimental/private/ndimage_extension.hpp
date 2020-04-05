#pragma once

#include <type_traits>
#include <cstddef>


namespace mln::internal
{
  template <typename T, unsigned dim>
  struct ndimage_extension
  {
  public:
    using value_type          = T;
    using support_fill        = std::true_type;
    using support_mirror      = std::true_type;
    using support_periodize   = std::false_type; // TODO
    using support_clamp       = std::false_type; // TODO
    using support_extend_with = std::false_type; // TODO

    ndimage_extension(char* ptr, const std::size_t strides[], const int dims[], int border);

    template <typename SE>
    bool fit(const SE& se) const;
    int  extent() const;
    void fill(const T& v) noexcept;
    void mirror(std::size_t padding = 0) noexcept;

    // TODO
    // void periodize() noexcept;
    // void clamp() noexcept;
    // template <typename U>
    // void extend_with(U&& u, point_type /*offset*/ = {}) noexcept;

    constexpr bool is_fill_supported() const;
    constexpr bool is_mirror_supported() const;
    constexpr bool is_periodize_supported() const;
    constexpr bool is_clamp_supported() const;
    constexpr bool is_extend_with_supported() const;

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
    typename std::enable_if<(d < dim)>::type _mirror(char* ptr, [[maybe_unused]] std::size_t padding);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _mirror(char* ptr, [[maybe_unused]] std::size_t padding);

    template <unsigned d>
    typename std::enable_if<(d < dim)>::type _copy_line(char* src, char* dest);

    template <unsigned d>
    typename std::enable_if<(d == dim)>::type _copy_line(char* src, char* dest);

  private:
    size_t m_strides[dim];
    int    m_dims[dim];
    char*  m_ptr;
    int    m_border;
  };


  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename T, unsigned dim>
  ndimage_extension<T, dim>::ndimage_extension(char* ptr, const std::size_t* strides, const int dims[], int border)
    : m_border(border)
  {
    m_ptr = ptr;
    for (unsigned i = 0; i < dim; ++i)
    {
      m_dims[i]     = dims[i];
      m_strides[i] = strides[i];
      m_ptr -= m_strides[i] * border;
    }
  }

  template <typename T, unsigned dim>
  template <typename SE>
  bool ndimage_extension<T, dim>::fit(const SE& se) const
  {
    static_assert(mln::is_a<SE, mln::experimental::StructuringElement>() || mln::is_a<SE, mln::Neighborhood>());

    if constexpr (std::is_convertible_v<typename SE::category, dynamic_neighborhood_tag>)
    {
      return se.radial_extent() <= m_border;
    }
    else
    {
      return false; // TODO check wether default value should be true false or error
    }
  }

  template <typename T, unsigned dim>

  int ndimage_extension<T, dim>::extent() const
  {
    return m_border;
  }

  template <typename T, unsigned dim>
  void ndimage_extension<T, dim>::fill(const T& v) noexcept
  {
    _fill<0>(m_ptr, v);
  }

  template <typename T, unsigned dim>
  void ndimage_extension<T, dim>::mirror(std::size_t padding) noexcept
  {
    _mirror<0>(m_ptr, padding);
  }

  template <typename T, unsigned dim>
  constexpr bool ndimage_extension<T, dim>::is_fill_supported() const
  {
    return support_fill::value;
  }

  template <typename T, unsigned dim>
  constexpr bool ndimage_extension<T, dim>::is_mirror_supported() const
  {
    return support_mirror::value;
  }

  template <typename T, unsigned dim>
  constexpr bool ndimage_extension<T, dim>::is_periodize_supported() const
  {
    return support_periodize::value;
  }

  template <typename T, unsigned dim>
  constexpr bool ndimage_extension<T, dim>::is_clamp_supported() const
  {
    return support_clamp::value;
  }

  template <typename T, unsigned dim>
  constexpr bool ndimage_extension<T, dim>::is_extend_with_supported() const
  {
    return support_extend_with::value;
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_mirror(char*                        ptr,
                                                                              [[maybe_unused]] std::size_t padding)
  {
    char* pori = ptr + m_border * m_strides[d];
    char* p    = pori;
    for (int i = 0; i < m_dims[d]; ++i, p += m_strides[d])
      _mirror<d + 1>(p, padding);

    char* src1  = pori;
    char* dest1 = pori - m_strides[d];
    char* dest2 = pori + m_dims[d] * m_strides[d];
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
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_mirror(char*                        ptr,
                                                                               [[maybe_unused]] std::size_t padding)
  {
    (void)ptr;
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d < dim)>::type ndimage_extension<T, dim>::_copy_line(char* src, char* dest)
  {
    for (int k = 0; k < m_dims[d] + 2 * m_border; ++k)
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

    for (int i = 0; i < m_dims[d]; ++i, ptr += m_strides[d])
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
    for (int i = 0; i < m_dims[d] + (2 * m_border); ++i, ptr += m_strides[d])
      _fillall<d + 1>(ptr, v);
  }

  template <typename T, unsigned dim>
  template <unsigned d>
  typename std::enable_if<(d == dim)>::type ndimage_extension<T, dim>::_fillall(char* ptr, const T& v)
  {
    *(reinterpret_cast<T*>(ptr)) = v;
  }


} // namespace mln::internal
