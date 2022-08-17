#pragma once
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <type_traits>

#include <ano/bp/utils.hpp>


namespace ano::bp
{

  /// \brief Swap 2 2D-buffer
  ///
  /// \rst
  /// :param a: Buffer A
  /// :param b: Buffer B
  /// :param width: Width of the 2D-buffers
  /// :param height: Height of the 2D-buffers
  /// :param astride: Stride of the buffer A (in bytes)
  /// :param astride: Stride of the buffer B (in bytes)
  /// \endrst
  template <class T>
  void swap(T* __restrict a, T* __restrict b, int width, int height, std::ptrdiff_t astride,
            std::ptrdiff_t bstride) noexcept(std::is_nothrow_swappable_v<T>);

  /// \brief Swap two 2D-buffers with bounds known at compile-time (mostly used for small block)
  template <int WIDTH, int HEIGHT, class T>
  void swap(T* __restrict a, T* __restrict b, void* __restrict tmp, int width, int height, std::ptrdiff_t a_stride,
            std::ptrdiff_t b_stride);

  /******************************************/
  /****          Imlementation          ****/
  /******************************************/

  namespace impl
  {
    template <class T>
    inline void swap_generic(T* __restrict a, T* __restrict b, int width, int height, std::ptrdiff_t a_stride,
                             std::ptrdiff_t b_stride) noexcept(std::is_nothrow_swappable_v<T>)
    {
      for (int y = 0; y < height; ++y)
      {
        auto a_ptr = ptr_offset(a, y * a_stride);
        auto b_ptr = ptr_offset(b, y * b_stride);
        std::swap_ranges(a_ptr, a_ptr + width, b_ptr);
      }
    }


    [[gnu::always_inline]] inline void swap_buffer(void* __restrict a, void* __restrict b, void* __restrict tmp,
                                                   std::size_t n)
    {
      std::memcpy(tmp, a, n);
      std::memcpy(a, b, n);
      std::memcpy(b, tmp, n);
    }


    [[gnu::noinline]] void swap_raw(std::byte* __restrict a, std::byte* __restrict b, std::size_t len, int h,
                                    std::ptrdiff_t a_stride, std::ptrdiff_t b_stride) noexcept;


  } // namespace impl


  // Generic implementation
  template <class T>
  void swap(T* __restrict a, T* __restrict b, int width, int height, //
            std::ptrdiff_t a_stride, std::ptrdiff_t b_stride) noexcept(std::is_nothrow_swappable_v<T>)
  {
    impl::swap_generic(a, b, width, height, a_stride, b_stride);
  }


  // Specialization of trivially copyable type
  template <class T>
  requires std::is_trivially_copyable_v<T>
  inline void swap(T* __restrict a, T* __restrict b, int width, int height, std::ptrdiff_t a_stride,
                   std::ptrdiff_t b_stride)
  {
    impl::swap_raw((std::byte*)a, (std::byte*)b, width * sizeof(T), height, a_stride, b_stride);
  }


  template <int WIDTH, int HEIGHT, class T>
  inline void swap(T* __restrict a, T* __restrict b, void* __restrict tmp, int width, int height,
                   std::ptrdiff_t a_stride, std::ptrdiff_t b_stride)
  {
    ano_assume(width <= WIDTH);
    ano_assume(height <= HEIGHT);

    for (int y = 0; y < height; ++y)
      impl::swap_buffer((std::byte*)a + y * a_stride, (std::byte*)b + y * b_stride, tmp, width * sizeof(T));
  }

} // namespace ano::bp
