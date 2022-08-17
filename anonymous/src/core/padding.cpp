#include <ano/core/extension/padding.hpp>

#include <cassert>

namespace
{

  void md_memset(ano::padder<void>* vs, int dim, std::byte* buffer, const int sizes[],
                 const std::ptrdiff_t byte_strides[], void* value) noexcept
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


  void md_memset(ano::padder<void>* vs, int dim, std::byte* buffer, const ano::Box& roi,
                 const std::ptrdiff_t byte_strides[], void* value) noexcept
  {
    constexpr int MAX_DIM = 16;
    assert(dim < MAX_DIM);

    int sizes[MAX_DIM];
    for (int i = 0; i < dim; ++i)
      sizes[i] = roi.size(i);
    md_memset(vs, dim, buffer, sizes, byte_strides, value);
  }


  void md_memcpy(ano::padder<void>* vs, int dim, std::byte* dst, const std::byte* src, const int sizes[],
                 const std::ptrdiff_t dst_byte_strides[], const std::ptrdiff_t src_byte_strides[]) noexcept
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
  void copy_and_pad_horizontally_2d(ano::padder<void>* vs, std::byte* dest, const std::byte* src, int width, int height,
                                    int a, int b, std::ptrdiff_t src_pitch, std::ptrdiff_t dest_pitch, std::size_t size,
                                    ano::e_padding_mode padding, void* value)
  {
    assert((b - a) > 0);
    assert(width > 0);


    // [a0,b0) is the range on src that will be copied (a subrange of [0,width) )
    // bl: is the width of the left border
    // br: is the width of the right border
    const int a0 = std::clamp(std::clamp(a, 0, width), a, b);
    const int b0 = std::clamp(std::clamp(b, 0, width), a, b);
    const int w0 = b0 - a0;
    const int bl = a0 - a;
    const int br = b - b0;
    assert(a <= a0 && a0 <= b0 && b0 <= b);


    for (int y = 0; y < height; ++y)
    {
      const std::byte* i_lineptr = src + y * src_pitch;
      std::byte*       o_lineptr = dest + y * dest_pitch;

      // 1. Pad left
      if (bl > 0)
      {
        switch (padding)
        {
        case ano::PAD_ZERO:
        case ano::PAD_CONSTANT:
          vs->memset(o_lineptr, bl, value);
          break;
        case ano::PAD_REPLICATE:
          vs->memset(o_lineptr, bl, i_lineptr + 0 * size);
          break;
        case ano::PAD_MIRROR:
          vs->mirror(o_lineptr, i_lineptr, a, a0, width);
          break;
        case ano::PAD_WRAP:
          vs->periodize(o_lineptr, i_lineptr, a, a0, width);
          break;
        }
        o_lineptr += bl * size;
      }

      if (w0 > 0)
        vs->memcpy(o_lineptr, i_lineptr + a0 * size, w0);
      o_lineptr += w0 * size;

      // 2. Pad right
      if (br > 0)
      {
        switch (padding)
        {
        case ano::PAD_ZERO:
        case ano::PAD_CONSTANT:
          vs->memset(o_lineptr, br, value);
          break;
        case ano::PAD_REPLICATE:
          vs->memset(o_lineptr, br, i_lineptr + (width - 1) * size);
          break;
        case ano::PAD_MIRROR:
          vs->mirror(o_lineptr, i_lineptr, b0, b, width);
          break;
        case ano::PAD_WRAP:
          vs->periodize(o_lineptr, i_lineptr, b0, b, width);
          break;
        }
      }
    }
  }


  void pad_horizontally_2d(ano::padder<void>* vs, std::byte* buffer, int width, int height, std::size_t size,
                           std::ptrdiff_t pitch, int bl, int br, ano::e_padding_mode padding, void* value)
  {

    // [a0,b0) is the range on src that will be copied (a subrange of [0,width) )
    // bl: is the width of the left border
    // br: is the width of the right border
    const int b  = width;
    const int a0 = bl;
    const int b0 = width - br;
    const int w0 = b0 - a0;


    for (int y = 0; y < height; ++y)
    {
      std::byte* lineptr = buffer + y * pitch;

      // 1. Pad left
      if (bl > 0)
      {
        switch (padding)
        {
        case ano::PAD_ZERO:
        case ano::PAD_CONSTANT:
          vs->memset(lineptr, bl, value);
          break;
        case ano::PAD_REPLICATE:
          vs->memset(lineptr, bl, lineptr + a0 * size);
          break;
        case ano::PAD_MIRROR:
          vs->mirror(lineptr, lineptr + a0 * size, -a0, 0, w0);
          break;
        case ano::PAD_WRAP:
          vs->periodize(lineptr, lineptr + a0 * size, -a0, 0, w0);
          break;
        }
      }

      // 2. Pad right
      if (br > 0)
      {
        switch (padding)
        {
        case ano::PAD_ZERO:
        case ano::PAD_CONSTANT:
          vs->memset(lineptr + b0 * size, br, value);
          break;
        case ano::PAD_REPLICATE:
          vs->memset(lineptr + b0 * size, br, lineptr + (b0 - 1) * size);
          break;
        case ano::PAD_MIRROR:
          vs->mirror(lineptr + b0 * size, lineptr + a0 * size, b0 - a0, b - a0, w0);
          break;
        case ano::PAD_WRAP:
          vs->periodize(lineptr + b0 * size, lineptr + a0 * size, b0 - a0, b - a0, w0);
          break;
        }
      }
    }
  }
} // namespace


namespace ano::impl
{

  void pad(ano::padder<void>* vs, int dim, std::byte* buffer, const int sizes[], const std::ptrdiff_t strides[],
           const int borders[][2], e_padding_mode padding, void* value) noexcept
  {
    const int            bl    = borders[dim - 1][0];
    const int            br    = borders[dim - 1][1];
    const int            n     = sizes[dim - 1];
    const int            a0    = bl;
    const int            b0    = n - br;
    const int            w0    = b0 - a0;
    const std::ptrdiff_t pitch = strides[dim - 1];


    if (dim == 1)
    {
      pad_horizontally_2d(vs, buffer, sizes[0], 1, strides[0], 0, bl, br, padding, value);
      return;
    }

    // Pad each element of the lower dim
    if (dim == 2)
    {
      pad_horizontally_2d(vs, buffer + a0 * pitch, sizes[0], w0, strides[0], strides[1], borders[0][0], borders[0][1],
                          padding, value);
    }
    else
    {
      for (int z = a0; z < b0; ++z)
        impl::pad(vs, dim - 1, buffer + pitch * z, sizes, strides, borders, padding, value);
    }

    // Pad upper border
    // Pad lower border
    auto mirror = [w0, a0](int x) {
      int n = w0 * 2;
      x     = (x - a0) % n;               // x in (-n:n)
      x     = (x >= 0) ? x : x + n;       // x in [0:n)
      x     = (x < w0) ? x : (n - x - 1); // x in [0,w0)
      return a0 + x;
    };
    auto periodize = [w0, a0](int x) {
      x = (x - a0) % w0;
      x = (x >= 0) ? x : x + w0;
      return a0 + x;
    };


    switch (padding)
    {
    case PAD_ZERO:
    case PAD_CONSTANT:
      for (int z = 0; z < a0; ++z)
        md_memset(vs, dim - 1, buffer + pitch * z, sizes, strides, value);
      for (int z = b0; z < n; ++z)
        md_memset(vs, dim - 1, buffer + pitch * z, sizes, strides, value);
      break;
    case PAD_REPLICATE:
      for (int z = 0; z < a0; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * a0, sizes, strides, strides);
      for (int z = b0; z < n; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * (b0 - 1), sizes, strides, strides);
      break;
    case PAD_MIRROR:
      for (int z = 0; z < a0; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * mirror(z), sizes, strides, strides);
      for (int z = b0; z < n; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * mirror(z), sizes, strides, strides);
      break;
    case PAD_WRAP:
      for (int z = 0; z < a0; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * periodize(z), sizes, strides, strides);
      for (int z = b0; z < n; ++z)
        md_memcpy(vs, dim - 1, buffer + pitch * z, buffer + pitch * periodize(z), sizes, strides, strides);
      break;
    }
  } // namespace
    // ano::implvoidpad(ano::padder<void>*vs,intdim,std::byte*buffer,constintsizes[],conststd::ptrdiff_tstrides[],constintborders[][2],e_padding_modepadding,void*value)noexcept


  void copy_pad(ano::padder<void>* vs, int dim, std::byte* dst, std::byte* src, ano::Box roi, const int sizes[],
                const std::ptrdiff_t dst_strides[], const std::ptrdiff_t src_strides[], ano::e_padding_mode padding,
                void* value) noexcept
  {
    const int a  = roi.tl()[dim - 1];
    const int b  = roi.br()[dim - 1];
    const int w  = sizes[dim - 1];
    const int a0 = std::clamp(std::clamp(a, 0, sizes[dim - 1]), a, b);
    const int b0 = std::clamp(std::clamp(b, 0, sizes[dim - 1]), a, b);
    const int w0 = b0 - a0;
    const int bl = a0 - a;

    // [a:a0] left-padding area
    // [a0:b0] copy area
    // [b0;b] right padding area
    assert(a <= a0 && a0 <= b0 && b0 <= b);


    // const int      br        = b - b0;
    std::ptrdiff_t src_pitch = src_strides[dim - 1];
    std::ptrdiff_t dst_pitch = dst_strides[dim - 1];
    std::size_t    esize     = src_strides[0];


    if (dim == 1)
    {
      copy_and_pad_horizontally_2d(vs, dst, src, sizes[0], 1, a, b, 0, 0, esize, padding, value);
      return;
    }

    // Copy-And-Pad each element of the lower dim
    if (w0 > 0)
    {
      auto _src = src + a0 * src_pitch;
      auto _dst = dst + bl * dst_pitch;

      if (dim == 2)
      {
        copy_and_pad_horizontally_2d(vs, _dst, _src, sizes[0], w0, roi.tl().x(), roi.br().x(), src_pitch, dst_pitch,
                                     esize, padding, value);
      }
      else
      {
        for (int z = a0; z < b0; ++z)
        {
          copy_pad(vs, dim - 1, _dst, _src, roi, sizes, dst_strides, src_strides, padding, value);
          _src += src_pitch;
          _dst += dst_pitch;
        }
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


    // 2. Padding Left
    for (int z = a; z < a0; ++z)
    {
      int q;

      switch (padding)
      {
      case ano::PAD_REPLICATE:
        q = 0;
        break;
      case ano::PAD_MIRROR:
        q = mirror(z);
        break;
      case ano::PAD_WRAP:
        q = periodize(z);
        break;
      case ano::PAD_ZERO:
      case ano::PAD_CONSTANT:
      default:
        md_memset(vs, dim - 1, dst + (z - a) * dst_pitch, roi, dst_strides, value);
        continue;
      }
      copy_pad(vs, dim - 1, dst + (z - a) * dst_pitch, src + q * src_pitch, roi, sizes, dst_strides, src_strides,
               padding, value);
    }

    // 2. Padding right
    for (int z = b0; z < b; ++z)
    {
      int q;

      switch (padding)
      {
      case ano::PAD_REPLICATE:
        q = sizes[dim - 1] - 1;
        break;
      case ano::PAD_MIRROR:
        q = mirror(z);
        break;
      case ano::PAD_WRAP:
        q = periodize(z);
        break;
      case ano::PAD_ZERO:
      case ano::PAD_CONSTANT:
      default:
        md_memset(vs, dim - 1, dst + (z - a) * dst_pitch, roi, dst_strides, value);
        continue;
      }
      copy_pad(vs, dim - 1, dst + (z - a) * dst_pitch, src + q * src_pitch, roi, sizes, dst_strides, src_strides,
               padding, value);
    }
  }

} // namespace ano::impl
