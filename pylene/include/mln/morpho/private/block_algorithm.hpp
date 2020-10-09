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
  requires (::ranges::cpp20::invocable<BinaryFunction, T, T>)
  void block_partial_sum_generic(const T* __restrict in, T* __restrict out, int width, int height, std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup)
  {
    std::copy_n(in, width, out);

    const T* prev = out;
    for (int y = 1; y < height; ++y)
    {
      const T* in_lineptr  = ptroffset(in, y * in_byte_stride);
      T*       out_lineptr = ptroffset(out, y * out_byte_stride);

      for (int c = 0; c < width; ++c)
        out_lineptr[c] = sup(prev[c], in_lineptr[c]);
    }
  }


  template <int BLOCK_WIDTH, class T, class BinaryFunction>
  void block_partial_sum(const T* __restrict in_, T* __restrict out_, int width, int height,
                         std::ptrdiff_t in_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction sup)
  {
    using simd_t = xsimd::simd_type<T>;


    constexpr std::size_t WARP_SIZE      = simd_t::size;
    constexpr int         WARP_PER_LINE  = BLOCK_WIDTH / WARP_SIZE;
    const int             K              = width / WARP_SIZE;
    const int             rem            = width % WARP_SIZE;

    // fmt::print("BLOCK_WIDTH={} width={}\n", BLOCK_WIDTH, width);
    // fmt::print("WARP_SIZE={}\n", WARP_SIZE);

    assert(width <= BLOCK_WIDTH);
    std::memcpy(out, in, sizeof(T) * width);

    // By block
    if (K > 0)
    {

      simd_t xsum[WARP_PER_LINE];

      for (int k = 0; k < K; k++)
        xsimd::load_aligned((T*)in + k * WARP_SIZE, xsum[k]);

      // Next lines
      for (int y = 1; y < height; ++y)
      {
        const T* in_lineptr  = ptroffset(in, y * in_byte_stride);
        T*       out_lineptr = ptroffset(out, y * out_byte_stride);

        for (int k = 0; k < K; k++)
        {
          simd_t v;
          xsimd::load_aligned(in_lineptr + k * WARP_SIZE, v);
          xsum[k] = sup(xsum[k], v);
          xsimd::store_aligned(out_lineptr + k * WARP_SIZE, xsum[k]);
        }
      }
    }

    // The last columns
    if (rem > 0)
    {
      block_partial_sum_generic(in + K * WARP_SIZE, out + K * WARP_SIZE, rem, heigth, in_byte_stride, out_byte_stride, sup);
    }
  }



  template <class T, class BinaryFunction>
  requires (::ranges::cpp20::invocable<BinaryFunction, T, T>)
  void block_transform_generic(const T* __restrict a, const T* __restrict b,  T* __restrict out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction op)
  {
    for (int y = 0; y < height; ++y)
    {
      const T* a_lineptr  = ptroffset(a, y * a_byte_stride);
      const T* b_lineptr  = ptroffset(b, y * b_byte_stride);
      T*       out_lineptr = ptroffset(out, y * out_byte_stride);
      std::transform(a_lineptr, a_lineptr + width, b_lineptr, out_lineptr, op);
    }
  }


  template <class T, class BinaryFunction>
  requires (::ranges::cpp20::invocable<BinaryFunction, T, T>)
  void block_transform(const T* __restrict a, const T* __restrict b,  T* __restrict out, int width, int height, std::ptrdiff_t a_byte_stride, std::ptrdiff_t b_byte_stride, std::ptrdiff_t out_byte_stride, BinaryFunction op)
  {
    using simd_t = xsimd::simd_type<T>;

    constexpr std::size_t WARP_SIZE      = simd_t::size;
    constexpr int         WARP_PER_LINE  = BLOCK_WIDTH / WARP_SIZE;
    const int             K              = width / WARP_SIZE;
    const int             rem            = width % WARP_SIZE;

     // By block
    if (K > 0)
    {
      for (int y = 1; y < height; ++y)
      {
        const T* a_lineptr  = ptroffset(a, y * a_byte_stride);
        const T* b_lineptr  = ptroffset(b, y * b_byte_stride);
        T*       out_lineptr = ptroffset(out, y * out_byte_stride);

        for (int k = 0; k < K; k++)
        {
          simd_t a = xsimd::load_aligned(a_lineptr + k * WARP_SIZE);
          simd_t b = xsimd::load_aligned(b_lineptr + k * WARP_SIZE);
          simd_t c = a + b;
          xsimd::store_aligned(out_lineptr + k * WARP_SIZE, c);
        }
      }
    }

     // The last columns
    if (rem > 0)
    {
      block_partial_sum_generic(a + K * WARP_SIZE, b + K * WARP_SIZE, c + K * WARP_SIZE, //
                                rem, heigth, a_byte_stride, b_byte_stride, out_byte_stride, sup);
    }

  }

}
