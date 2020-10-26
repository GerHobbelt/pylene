#pragma once


#include <mln/core/utils/ptroffset.hpp>
#include <mln/core/box.hpp>

#include <cstddef>
#include <cassert>
#include <algorithm>


#include <range/v3/functional/concepts.hpp>
#include <xsimd/xsimd.hpp>




///
/// Algorithms that process a block column-wise with simd acceleration when possible

namespace mln::morpho::details
{
  constexpr int MAX_BLOCK_WIDTH = 128;

  // Apply the partial sum algorithm column-wise on a block
  template <class T, class BinaryFunction>
  void block_partial_sum(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup, T zero) noexcept;


  // Apply the partial sum algorithm column-wise on a block
  template <class T, class BinaryFunction>
  void block_transform(const T* __restrict a, const T* __restrict b,  T* __restrict out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction op) noexcept;


  // Transpose a square block inplace
  template <class T>
  void block_transpose(T* in, int width, std::ptrdiff_t byte_stride) noexcept;

  // Transpose a block
  // Input is supposed to be located at (0,0)
  // Coordinate of the output roi are relative to (0,0)
  // Equivalent to:
  // out(p - roi.tl()) = in({p.y, p.x})
  template <class T>
  void block_transpose(const T* in, T* out, int width, int height, std::ptrdiff_t in_byte_stride,
                       std::ptrdiff_t out_byte_stride) noexcept;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class T, class BinaryFunction>
  [[gnu::noinline]] //
  void
  block_partial_sum(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride,
                    std::ptrdiff_t out_byte_stride, BinaryFunction sup, T /*zero*/) noexcept
  {
    static_assert(::ranges::cpp20::invocable<BinaryFunction, T, T>);

    std::copy_n(in, width, out);

    const T* prev = out;
    for (int y = 1; y < height; ++y)
    {
      const T* in_lineptr  = ptr_offset(in, y * in_byte_stride);
      T*       out_lineptr = ptr_offset(out, y * out_byte_stride);

      for (int c = 0; c < width; ++c)
        out_lineptr[c] = sup(prev[c], in_lineptr[c]);
      prev = out_lineptr;
    }
  }


  template <::concepts::integral T, class BinaryFunction>
  [[gnu::noinline]] //
  void
  block_partial_sum(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride,
                    std::ptrdiff_t out_byte_stride, BinaryFunction sup, T zero) noexcept
  {
    // For bool, consider the corresponding uint8_t type
    using U = std::conditional_t<std::is_same_v<T, bool>, std::uint8_t, T>;
    using simd_t = xsimd::simd_type<U>;
    static_assert(::ranges::cpp20::invocable<BinaryFunction, U, U>);
    static_assert(::ranges::cpp20::invocable<BinaryFunction, simd_t, simd_t>);

    constexpr std::size_t WARP_SIZE     = simd_t::size;
    const int             WARP_PER_LINE = (width + (WARP_SIZE - 1)) / WARP_SIZE;
    const int             K             = width / WARP_SIZE;
    const int             rem           = width % WARP_SIZE;

    {
      simd_t* xsum = new simd_t[WARP_PER_LINE];

      for (int k = 0; k < WARP_PER_LINE; k++)
        xsum[k] = simd_t{zero};


      for (int y = 0; y < height; ++y)
      {
        const U* in_lineptr  = (const U*)ptr_offset(in, y * in_byte_stride);
        U*       out_lineptr = (U*)ptr_offset(out, y * out_byte_stride);


        for (int k = 0; k < K; k++)
        {
          simd_t v;
          xsimd::load_unaligned(in_lineptr + k * WARP_SIZE, v);
          xsum[k] = sup(xsum[k], v);
          xsimd::store_unaligned(out_lineptr + k * WARP_SIZE, xsum[k]);
        }

        in_lineptr += K * WARP_SIZE;
        out_lineptr += K * WARP_SIZE;

        // The last columns
        for (int k = 0; k < rem; ++k)
        {
          xsum[WARP_PER_LINE - 1][k] = sup(in_lineptr[k], xsum[WARP_PER_LINE-1][k]);
          out_lineptr[k] = xsum[WARP_PER_LINE-1][k];
        }
      }
      delete [] (xsum);
    }
  }


  /******************************************/
  /****         Block transform          ****/
  /******************************************/


  template <class T, class BinaryFunction>
  [[gnu::noinline]] //
  void
  block_transform(const T* __restrict a, const T* __restrict b, T* __restrict out, int width, int height,
                  std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride,
                  BinaryFunction op) noexcept
  {
    static_assert(::ranges::cpp20::invocable<BinaryFunction, T, T>);
    for (int y = 0; y < height; ++y)
    {
      const T* a_lineptr  = ptr_offset(a, y * a_byte_stride);
      const T* b_lineptr  = ptr_offset(b, y * b_byte_stride);
      T*       out_lineptr = ptr_offset(out, y * out_byte_stride);
      std::transform(a_lineptr, a_lineptr + width, b_lineptr, out_lineptr, op);
    }
  }


      // Simd version
  template <::concepts::integral T, class BinaryFunction>
  [[gnu::noinline]] //
  void
  block_transform(const T* __restrict a, const T* __restrict b, T* __restrict out, int width, int height,
                  std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride,
                  BinaryFunction op) noexcept
  {
    // For bool, consider the corresponding uint8_t type
    using U = std::conditional_t<std::is_same_v<T, bool>, std::uint8_t, T>;
    using simd_t = xsimd::simd_type<U>;
    static_assert(::ranges::cpp20::invocable<BinaryFunction, T, T>);
    static_assert(::ranges::cpp20::invocable<BinaryFunction, simd_t, simd_t>);


    constexpr std::size_t WARP_SIZE = simd_t::size;
    const int             K         = width / WARP_SIZE;


    for (int y = 0; y < height; ++y)
    {
      const U* a_lineptr   = (const U*)ptr_offset(a, y * a_byte_stride);
      const U* b_lineptr   = (const U*)ptr_offset(b, y * b_byte_stride);
      U*       out_lineptr = (U*)ptr_offset(out, y * out_byte_stride);

      for (int k = 0; k < K; k++)
      {
        simd_t a = xsimd::load_unaligned(a_lineptr + k * WARP_SIZE);
        simd_t b = xsimd::load_unaligned(b_lineptr + k * WARP_SIZE);
        simd_t c = op(a, b);
        xsimd::store_unaligned(out_lineptr + k * WARP_SIZE, c);
      }

      // The last columns
      for (int k = K * WARP_SIZE; k < width; ++k)
        out_lineptr[k] = op(a_lineptr[k], b_lineptr[k]);
    }
  }


  // Transpose a square block inplace (generic version)
  template <class T>
  [[gnu::noinline]]
  void block_transpose(T* in, int width, std::ptrdiff_t byte_stride) noexcept
  {
    for (int y = 0; y < width; ++y)
    {
      T* lineptr = ptr_offset(in, y * byte_stride);
      T* colptr  = in + y;
      for (int x = y + 1; x < width; ++x)
        *ptr_offset(colptr, x * byte_stride) = lineptr[x];
    }
  }


  // SIMD version of transposition
  #if XSIMD_INSTR_SET

  // Transpose inplace of a 4x4 matrix of DWORD data
  [[gnu::noinline]]
  void block_transpose_4x4(const std::byte* __restrict in, std::byte* __restrict out, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) noexcept
  {
    xsimd::simd<int32_t, 4> rows[4];
    rows[0].load_unaligned((int32_t*)(in + 0 * in_byte_stride));
    rows[1].load_unaligned((int32_t*)(in + 1 * in_byte_stride));
    rows[2].load_unaligned((int32_t*)(in + 2 * in_byte_stride));
    rows[3].load_unaligned((int32_t*)(in + 3 * in_byte_stride));

    _MM_TRANSPOSE4_PS(rows[0], rows[1], rows[2], rows[3]);

    rows[0].store_unaligned((int32_t*)(out + 0 * out_byte_stride));
    rows[1].store_unaligned((int32_t*)(out + 1 * out_byte_stride));
    rows[2].store_unaligned((int32_t*)(out + 2 * out_byte_stride));
    rows[3].store_unaligned((int32_t*)(out + 3 * out_byte_stride));
  }

  template <class T>
  [[gnu::noinline]]
  void block_transpose_generic(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) noexcept
  {
    for (int y = 0; y < height; ++y)
    {
      T*       lineptr = ptr_offset(out, y * out_byte_stride);
      const T* colptr  = in + y;
      for (int x = 0; x < width; ++x)
        lineptr[x] = *ptr_offset(colptr, x * in_byte_stride);
    }

  }


  template <::concepts::integral T>
  [[gnu::noinline]]
  void block_transpose(const T* in, T* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) noexcept
  {
    constexpr int block_size = 128 / sizeof(T);

    int yrem = height;


    for (int y = 0; yrem >= block_size; y += block_size, yrem -= block_size)
    {
      int xrem = width;
      for (int x = 0; xrem >= block_size; x += block_size, xrem -= block_size)
      {
        const T* iBlock = ptroffset(in, x * in_byte_stride) + y;
        T* oBlock = ptroffset(out, y * out_byte_stride) + x;
        block_transpose_4x4((const std::byte*) iBlock, (const std::byte*) oBlock, in_byte_stride, out_byte_stride);
      }

      if (xrem >= 0)
      {
        const T* iBlock = ptroffset(in, x * in_byte_stride) + y;
        T*       oBlock = ptroffset(out, y * out_byte_stride) + x;
        block_transpose_generic(iBlock, oBlock, xrem, block_size, in_byte_stride, out_byte_stride);
      }
    }
    if (yrem >= 0)
    {
      const T* iBlock = ptroffset(in, x * in_byte_stride) + y;
      T*       oBlock = ptroffset(out, y * out_byte_stride) + x;
      block_transpose_generic(iBlock, oBlock, width, yrem, in_byte_stride, out_byte_stride);
    }
  }

  #endif


  template <class T>
  void block_transpose(const T* in, T* out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride) noexcept
  {
    block_transpose_generic(in, out, width, height, in_byte_stride, out_byte_stride);
  }



} // namespace mln::morpho::details
