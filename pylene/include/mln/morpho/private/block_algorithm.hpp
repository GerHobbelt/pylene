#pragma once


#include <mln/core/utils/ptroffset.hpp>

#include <cstddef>
#include <algorithm>


#include <range/v3/functional/concepts.hpp>
#include <xsimd/xsimd.hpp>

///
/// Algorithms that process a block column-wise with simd acceleration when possible

namespace mln::morpho::details
{

  // Apply the partial sum algorithm column-wise on a block
  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_partial_sum(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup);


  // Apply the partial sum algorithm column-wise on a block
  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_transform(const T* __restrict a, const T* __restrict b,  T* __restrict out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction op);




  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class T, class BinaryFunction>
  [[gnu::noinline]]
  void block_partial_sum_generic(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup)
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


  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  [[gnu::noinline]] void block_partial_sum(const T* __restrict in, T* __restrict out, int width, int height,
                                           std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride,
                                           BinaryFunction sup)
  {
    using simd_t = xsimd::simd_type<T>;


    constexpr std::size_t WARP_SIZE     = simd_t::size;
    constexpr int         WARP_PER_LINE = BLOCK_WIDTH / WARP_SIZE;
    const int             K             = width / WARP_SIZE;
    const int             rem           = width % WARP_SIZE;

    assert(width <= BLOCK_WIDTH);
    std::memcpy(out, in, sizeof(T) * width);

    {
      simd_t xsum[WARP_PER_LINE];
      T      xsum_s[WARP_SIZE];

      for (int k = 0; k < K; k++)
        xsimd::load_unaligned((T*)in + k * WARP_SIZE, xsum[k]);
      for (int k = 0; k < rem; ++k)
        xsum_s[k] = in[K * WARP_SIZE + k];

      // Next lines
      for (int y = 1; y < height; ++y)
      {
        const T* in_lineptr  = ptr_offset(in, y * in_byte_stride);
        T*       out_lineptr = ptr_offset(out, y * out_byte_stride);

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
          xsum_s[k]       = sup(in_lineptr[k], xsum_s[k]);
          out_lineptr[+k] = xsum_s[k];
        }
      }
    }
  }


  template <class T, class BinaryFunction>
  [[gnu::noinline]] //
  void
  block_transform_generic(const T* __restrict a, const T* __restrict b, T* __restrict out, int width, int height,
                          std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride,
                          BinaryFunction op)
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


  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  [[gnu::noinline]] void block_transform(const T* __restrict a, const T* __restrict b, T* __restrict out, int width,
                                         int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride,
                                         std::ptrdiff_t out_byte_stride, BinaryFunction op)
  {
    using simd_t = xsimd::simd_type<T>;
    static_assert(::ranges::cpp20::invocable<BinaryFunction, T, T>);
    static_assert(::ranges::cpp20::invocable<BinaryFunction, simd_t, simd_t>);


    assert(width < BLOCK_WIDTH);

    constexpr std::size_t WARP_SIZE = simd_t::size;
    const int             K         = width / WARP_SIZE;


    for (int y = 0; y < height; ++y)
    {
      const T* a_lineptr   = ptr_offset(a, y * a_byte_stride);
      const T* b_lineptr   = ptr_offset(b, y * b_byte_stride);
      T*       out_lineptr = ptr_offset(out, y * out_byte_stride);

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

} // namespace mln::morpho::details
