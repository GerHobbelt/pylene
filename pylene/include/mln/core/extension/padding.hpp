#pragma once

#include <cstddef>
#include <algorithm>

#include <mln/core/image/ndimage_fwd.hpp>
#include <mln/core/range/mdspan_fwd.hpp>


namespace mln
{
  enum e_padding_mode
  {
    PAD_ZERO,
    PAD_CONSTANT,
    PAD_WRAP,
    PAD_MIRROR,
    PAD_REPLICATE,
  };

  /// \brief Pad an ndimage or a mdspan in-place using one of the padding strategy
  ///
  /// Given `[0 0 1 2 3 0 0 0 0]`, borders = `{2, 4}` and value = `9`
  ///
  /// PAD_ZERO
  ///   pad each dimension with value zero `[0 0 1 2 3 0 0 0 0]`
  /// PAD_CONSTANT
  ///   pad each dimension with \p value `[9 9 1 2 3 9 9 9 9]`
  /// PAD_WRAP
  ///   pad each dimension using periodization `[2 3 1 2 3 1 2 3 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge mirroring `[2 1 1 2 3 3 2 1 1]`
  /// PAD_MIRROR
  ///   pad each dimension using edge replication `[1 1 1 2 3 3 3 3 3]`
  ///
  /// \param input The image or mdspan to pad
  /// \param mode The padding mode
  /// \param borders The padding region express as distances from the border
  /// \param value The value uses when mode=PAD_CONSTANT
  template <class T, int dim>
  void pad(__ndbuffer_image<T, dim>& image, e_padding_mode mode, const int borders[][2], T value = {});

  /// \overload
  template <class T, std::size_t dim>
  void pad(const ranges::mdspan<T, dim>& image, e_padding_mode mode, const int borders[][2], T value = {});

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class T>
  class mdspan_padder;

  template <>
  class mdspan_padder<void>;


  // Type erased implementation
  template <>
  class mdspan_padder<void>
  {
    static constexpr int _max_dim = 4;
  public:
    mdspan_padder(void* buffer, int dim, const int sizes[], const std::ptrdiff_t strides[], const int borders[][2]) noexcept;

  protected:
    void _pad(e_padding_mode padding, void* value) const noexcept;
    void _pad(int dim, char* buffer, e_padding_mode padding, void* value) const noexcept;
    void memset(int dim, char* buffer, void* value) const noexcept;
    void memcpy(int dim, char* dst, char* src) const noexcept;

    virtual void pad_horizontally_2d(char* buffer, e_padding_mode padding, void* value) const noexcept = 0;
    virtual void memset(void* buffer, std::size_t n, void* value) const noexcept = 0;
    virtual void memcpy(void* dst, void* src, std::size_t n) const noexcept = 0;

    // FIXME, consider using boost::small_vector to break the limitation to _max_dim;
    char*           m_buffer;
    std::ptrdiff_t  m_byte_strides[_max_dim];
    int             m_sizes[_max_dim];
    int             m_borders[_max_dim][2]; // left/right padding in each dimension
    int             m_dim;
  };


  /// \brief This class offers padding facilities over a multidimensional span
  template <class T>
  class mdspan_padder : mdspan_padder<void>
  {
  public:
    /// \brief
    ///
    /// \param buffer  Pointer over the beginning of the multidimensional buffer
    /// \param dim     Number of dimensions of the buffer
    /// \param sizes   Dimensions of the buffer
    /// \param strides Number of bytes between two consecutive element
    /// \param borders Padding region parameters
    mdspan_padder(T* buffer, int dim, const int sizes[], const std::ptrdiff_t byte_strides[], const int borders[][2]) noexcept
      : mdspan_padder<void>((void*)buffer, dim, sizes, byte_strides, borders)
    {
    }

    /// Custom padding
    ///
    /// \param padding padding mode (one of zero, constant, periodize, mirror, replicate)
    /// \param value The value used to pad when mode is CONSTANT (default is zero)
    void pad(e_padding_mode padding, T value) const noexcept;

    // Constant padding with a value
    void pad(T value) const noexcept;

  private:
    void pad_horizontally_2d(char* buffer, e_padding_mode padding, void* value) const noexcept final;
    void memset(void* buffer, std::size_t n, void* value) const noexcept final;
    void memcpy(void* dst, void* src, std::size_t n) const noexcept final;
  };



  template <class T>
  void mdspan_padder<T>::pad(T value) const noexcept
  {
    this->_pad(PAD_CONSTANT, &value);
  }

  template <class T>
  void mdspan_padder<T>::pad(e_padding_mode padding, T value) const noexcept
  {
    this->_pad(padding, &value);
  }


  template <class T>
  void mdspan_padder<T>::memset(void* buffer, std::size_t n, void* value) const noexcept
  {
    T val = value ? *(T*)value : 0;
    std::fill_n(reinterpret_cast<T*>(buffer), n, val);
  }

  template <class T>
  void mdspan_padder<T>::memcpy(void* dst, void* src, std::size_t n) const noexcept
  {
    std::copy_n((T*)src, n, (T*)dst);
  }



  template <class T>
  void mdspan_padder<T>::pad_horizontally_2d(char* buffer, e_padding_mode padding, void* val) const noexcept
  {
    const auto w     = m_sizes[0];
    const auto h     = m_sizes[1];
    const auto pitch = m_byte_strides[1];
    const int  y0    = m_borders[1][0];
    const int  y1    = h - m_borders[1][1];
    const int  x0    = m_borders[0][0];
    const int  x1    = w - m_borders[0][1];
    const int  w0    = x1 - x0;


    T value = (val == nullptr) ? 0 : *(reinterpret_cast<const T*>(val));

    auto mirror = [w0, x0](int x) {
      int n = w0 * 2;
      x     = (x - x0) % n;               // x in (-n:n)
      x     = (x >= 0) ? x : x + n;       // x in [0:n)
      x     = (x < w0) ? x : (n - x - 1); // x in [0,w0)
      return x0 + x;
    };
    auto periodize = [w0, x0](int x) {
      x = (x - x0) % w0;
      x = (x >= 0) ? x : x + w0;
      return x0 + x;
    };


    switch (padding)
    {
    case PAD_ZERO:
    case PAD_CONSTANT:
      for (int y = y0; y < y1; y++)
      {
        T* lineptr = reinterpret_cast<T*>(buffer + y * pitch);
        std::fill(lineptr, lineptr + x0, value);
        std::fill(lineptr + x1, lineptr + w, value);
      }
      break;

    case PAD_REPLICATE:
      for (int y = y0; y < y1; y++)
      {
        T* lineptr = reinterpret_cast<T*>(buffer + y * pitch);
        std::fill(lineptr, lineptr + x0, lineptr[x0]);
        std::fill(lineptr + x1, lineptr + w, lineptr[x1 - 1]);
      }
      break;
    case PAD_MIRROR:
      for (int y = y0; y < y1; y++)
      {
        T* lineptr = reinterpret_cast<T*>(buffer + y * pitch);
        for (int x = 0; x < x0; ++x)
          lineptr[x] = lineptr[mirror(x)];
        for (int x = x1; x < w; ++x)
          lineptr[x] = lineptr[mirror(x)];
      }
      break;
    case PAD_WRAP:
      for (int y = y0; y < y1; y++)
      {
        T* lineptr = reinterpret_cast<T*>(buffer + y * pitch);
        for (int x = 0; x < x0; ++x)
          lineptr[x] = lineptr[periodize(x)];
        for (int x = x1; x < w; ++x)
          lineptr[x] = lineptr[periodize(x)];
      }
      break;
    }
  }


  template <class T, std::size_t dim>
  void pad(const ranges::mdspan<T, dim>& sp, e_padding_mode mode, const int borders[][2], T value)
  {
    std::ptrdiff_t strides[dim];
    int            sizes[dim];

    for (std::size_t i = 0; i < dim; ++i)
    {
      sizes[i] = static_cast<int>(sp.size(i));
      strides[i] = sp.byte_stride(i);
    }

    auto padder = mdspan_padder(sp.data(), static_cast<int>(dim), sizes, strides, borders);
    padder.pad(mode, value);
  }

  template <class T, int dim>
  void pad(__ndbuffer_image<T, dim>& f, e_padding_mode mode, const int borders[][2], T value)
  {
    std::ptrdiff_t strides[dim];
    int            sizes[dim];

    for (std::size_t i = 0; i < dim; ++i)
    {
      sizes[i] = static_cast<int>(f.size(i));
      strides[i] = f.byte_stride(i);
    }

    auto padder = mdspan_padder(f.buffer(), static_cast<int>(dim), sizes, strides, borders);
    padder.pad(mode, value);
  }

} // namespace mln
