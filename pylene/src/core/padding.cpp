#include <mln/core/extension/padding.hpp>

#include <cassert>

namespace
{

  void md_memset(mln::padder<void>* vs, int dim, std::byte* buffer, const int sizes[], const std::ptrdiff_t byte_strides[],
                 void* value) noexcept
  {
    if (dim == 1)
    {
      vs->memset(buffer, sizes[0], value);
      return;
    }

    std::ptrdiff_t stride = byte_strides[dim - 1];
    int            size   = sizes[dim - 1];

    for (int z = 0; z < size; ++z)
      md_memset(vs, dim - 1, buffer + stride * z, sizes, byte_strides, value);
  }


  void md_memcpy(mln::padder<void>* vs, int dim, std::byte* dst, const std::byte* src, const int sizes[], const std::ptrdiff_t dst_byte_strides[],
                 const std::ptrdiff_t src_byte_strides[]) noexcept
  {
    if (dim == 1)
    {
      vs->memcpy(dst, src, sizes[0]);
      return;
    }

    std::ptrdiff_t src_stride = src_byte_strides[dim - 1];
    std::ptrdiff_t dst_stride = dst_byte_strides[dim - 1];
    int            size       = sizes[dim - 1];

    for (int z = 0; z < size; ++z)
      md_memcpy(vs, dim - 1, dst + dst_stride * z, src + src_stride * z, sizes, dst_byte_strides, src_byte_strides);
  }


  /// Copy and pad
  /// \param vs: value set
  /// \param dest destination buffer of size: (b-a) x keight
  /// \param src source buffer of size: width x height
  /// \param width Width of the image
  /// \param height Height of the imahe
  /// \param a,b Interval (X) to be generated
  /// \param src_pitch
  /// \param dest_pitch
  /// \param size Size of an element
  /// \param padding Padding mode
  /// \param value Pointer to the padding value if value mode = CONSTANT
  void copy_and_pad_horizontally_2d(mln::padder<void>* vs, void* dest, void* src, int width, int height, int a, int b,
                                    std::ptrdiff_t src_pitch, std::ptrdiff_t dest_pitch, std::size_t size,
                                    mln::e_padding_mode padding, void* value)
  {
    assert((b - a) > 0);
    assert(width > 0);


    // [a0,b0) is the range on src that will be copied (a subrange of [0,width) )
    // bl: is the width of the left border
    // br: is the width of the right border
    const int a0 = std::max(a, 0);
    const int b0 = std::min(b, width);
    const int w0 = b0 - a0;
    const int bl = a0 - a;
    const int br = b - b0;


    for (int y = 0; y < height; ++y)
    {
      const std::byte* i_lineptr = (const std::byte*)src + y * src_pitch;
      std::byte*       o_lineptr = (std::byte*)dest + y * dest_pitch;

      // 1. Pad left
      if (bl > 0)
      {
        switch (padding)
        {
        case mln::PAD_ZERO:
        case mln::PAD_CONSTANT:
          vs->memset(o_lineptr, bl, value);
          break;
        case mln::PAD_REPLICATE:
          vs->memcpy(o_lineptr, i_lineptr, bl);
          break;
        case mln::PAD_MIRROR:
          vs->mirror(o_lineptr, i_lineptr, a, a0, width);
          break;
        case mln::PAD_WRAP:
          vs->periodize(o_lineptr, i_lineptr, a, a0, width);
          break;
        }
        o_lineptr += bl * size;
      }

      vs->memcpy(o_lineptr, i_lineptr + a0 * size, w0);
      o_lineptr += w0 * size;

      // 2. Pad right
      if (br > 0)
      {
        switch (padding)
        {
        case mln::PAD_ZERO:
        case mln::PAD_CONSTANT:
          vs->memset(o_lineptr, br, value);
          break;
        case mln::PAD_REPLICATE:
          vs->memcpy(o_lineptr, i_lineptr + (width - 1) * size, br);
          break;
        case mln::PAD_MIRROR:
          vs->mirror(o_lineptr, i_lineptr, b0, b, width);
          break;
        case mln::PAD_WRAP:
          vs->periodize(o_lineptr, i_lineptr, b0, b, width);
          break;
        }
      }
    }
  }


  void copy_and_pad(mln::padder<void>* vs, int dim, std::byte* dst, std::byte* src, mln::Box roi, const int sizes[],
                    const std::ptrdiff_t dst_strides[], const std::ptrdiff_t src_strides[], std::size_t esize,
                    mln::e_padding_mode padding, void* value)
  {
    const int      a         = roi.tl()[dim - 1];
    const int      b         = roi.br()[dim - 1];
    const int      w         = sizes[dim - 1];
    const int      a0        = std::max(0, a);
    const int      b0        = std::min(sizes[dim - 1], b);
    const int      bl        = a0 - a;
    std::ptrdiff_t src_pitch = src_strides[dim - 1];
    std::ptrdiff_t dst_pitch = dst_strides[dim - 1];


    if (dim == 1)
    {
      copy_and_pad_horizontally_2d(vs, dst, src, sizes[0], 1, a, b, 0, 0, esize, padding, value);
      return;
    }

    // Copy-And-Pad each element of the lower dim
    if (dim == 2)
    {
      copy_and_pad_horizontally_2d(vs, dst, src, sizes[0], sizes[1], roi.tl().x(), roi.br().x(), src_pitch, dst_pitch,
                                   esize, padding, value);
    }
    else
    {
      auto _src = src + a0 * src_pitch;
      auto _dst = dst + bl * dst_pitch;

      for (int z = a0; z < b0; ++z)
      {
        copy_and_pad(vs, dim - 1, _dst, _src, roi, sizes, dst_strides, src_strides, esize, padding, value);
        _src += src_pitch;
        _dst += dst_pitch;
      }
    }

    auto mirror = [w](int x) {
      int n = w * 2;
      x     = x % n;                     // x in (-n:n)
      x     = (x >= 0) ? x : x + n;      // x in [0:n)
      x     = (x < w) ? x : (n - x - 1); // x in [0,w)
      return x;
    };


    auto periodize = [w](int x) {
      x = x % w;
      x = (x >= 0) ? x : x + w;
      return x;
    };


    // 1. Pad left
    switch (padding)
    {
    case mln::PAD_ZERO:
    case mln::PAD_CONSTANT:
      for (int z = a; z < a0; ++z)
        md_memset(vs, dim - 1, dst + (z - a) * dst_pitch, sizes, dst_strides, value);
      for (int z = b0; z < b; ++z)
        md_memset(vs, dim - 1, dst + (z - a) * dst_pitch, sizes, dst_strides, value);
      break;
    case mln::PAD_REPLICATE:
      for (int z = a; z < a0; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + a0 * src_pitch, sizes, dst_strides, src_strides);
      for (int z = b0; z < b; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + (b0 - 1) * src_pitch, sizes, dst_strides, src_strides);
      break;
    case mln::PAD_MIRROR:
      for (int z = a; z < a0; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + mirror(z) * src_pitch, sizes, dst_strides, src_strides);
      for (int z = b0; z < b; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + mirror(z) * src_pitch, sizes, dst_strides, src_strides);
      break;
    case mln::PAD_WRAP:
      for (int z = a; z < a0; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + periodize(z) * src_pitch, sizes, dst_strides,
                  src_strides);
      for (int z = b0; z < b; ++z)
        md_memcpy(vs, dim - 1, dst + (z - a) * dst_pitch, src + periodize(z) * src_pitch, sizes, dst_strides,
                  src_strides);
      break;
    }
  }
} // namespace



namespace mln
{


  mdspan_padder<void>::mdspan_padder(void* buffer, int dim, const int sizes[], const std::ptrdiff_t strides[],
                                     const int borders[][2]) noexcept
    : m_buffer{(char*)buffer}
    , m_dim{dim}
  {
    assert(dim < _max_dim && "dim is currently limited to _max_dim");

    for (int k = 0; k < dim; ++k)
    {
      m_sizes[k]        = sizes[k];
      m_byte_strides[k] = strides[k];
      m_borders[k][0]   = borders[k][0];
      m_borders[k][1]   = borders[k][1];
      assert(m_borders[k][0] >= 0);
      assert(m_borders[k][1] >= 0);
      assert((m_borders[k][0] + m_borders[k][1]) < (int)sizes[k]);
    }
    for (int k = dim; k < _max_dim; ++k)
    {
      m_sizes[k]        = 1;
      m_byte_strides[k] = m_byte_strides[k - 1];
      m_borders[k][0]   = 0;
      m_borders[k][1]   = 0;
    }
  }

  void mdspan_padder<void>::_pad(e_padding_mode padding, void* value) const noexcept
  {
    if (padding == mln::PAD_ZERO)
      value = nullptr;
    this->_pad(m_dim, m_buffer, padding, value);
  }

  void mdspan_padder<void>::_pad(int dim, char* buffer, e_padding_mode padding, void* value) const noexcept
  {
    assert(dim <= m_dim);

    const auto n  = m_sizes[dim - 1];
    const int  a  = m_borders[dim - 1][0];
    const int  b  = n - m_borders[dim - 1][1];
    const int  w0 = b - a;


    if (dim == 1)
    {
      this->pad_horizontally_2d(buffer, padding, value);
      return;
    }

    // Pad each element of the lower dim
    if (dim == 2)
    {
      this->pad_horizontally_2d(buffer, padding, value);
    }
    else
    {
      for (int z = a; z < b; ++z)
        this->_pad(dim - 1, buffer + m_byte_strides[dim - 1] * z, padding, value);
    }

    // Pad upper border
    // Pad lower border
    auto mirror = [w0, a](int x) {
      int n = w0 * 2;
      x     = (x - a) % n;                // x in (-n:n)
      x     = (x >= 0) ? x : x + n;       // x in [0:n)
      x     = (x < w0) ? x : (n - x - 1); // x in [0,w0)
      return a + x;
    };
    auto periodize = [w0, a](int x) {
      x = (x - a) % w0;
      x = (x >= 0) ? x : x + w0;
      return a + x;
    };


    switch (padding)
    {
    case PAD_ZERO:
    case PAD_CONSTANT:
      for (int z = 0; z < a; ++z)
        this->memset(dim - 1, buffer + m_byte_strides[dim - 1] * z, value);
      for (int z = b; z < n; ++z)
        this->memset(dim - 1, buffer + m_byte_strides[dim - 1] * z, value);
      break;
    case PAD_REPLICATE:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * a);
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * (b - 1));
      break;
    case PAD_MIRROR:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * mirror(z));
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * mirror(z));
      break;
    case PAD_WRAP:
      for (int z = 0; z < a; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * periodize(z));
      for (int z = b; z < n; ++z)
        this->memcpy(dim - 1, buffer + m_byte_strides[dim - 1] * z, buffer + m_byte_strides[dim - 1] * periodize(z));
      break;
    }
  }





} // namespace mln
