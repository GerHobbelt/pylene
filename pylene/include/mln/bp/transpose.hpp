#pragma once

#include <mln/bp/utils.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>


namespace mln::bp
{


  /// \brief Transpose inplace a 2D array of size n × n
  ///
  /// The lines must be a correctly aligned on 32 bytes boundaries
  /// (consider padding the data if this is not the case)
  ///
  /// The implementation is optimized with simd instructions when possible
  template <class T>
  void transpose_inplace(T* buffer, int n, std::ptrdiff_t byte_stride) noexcept(std::is_nothrow_swappable_v<T>);


  /// @brief Transpose the data from \p src into \p dst
  ///
  /// @tparam T
  /// @param src Source buffer
  /// @param dst Destination buffer
  /// @param width Width of the *destination* buffer
  /// @param height Height of the *destination* buffer
  /// @param src_stride Pitch (in bytes) of the source buffer
  /// @param dst_stride Pitch (in bytes) of the destination buffer
  template <class T>
  void transpose(const T* src, T* dst, int width, int height, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept(std::is_nothrow_swappable_v<T>);


  void transpose_inplace(uint8_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(int8_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(uint16_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(int16_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(uint32_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(int32_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(uint64_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(int64_t* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(float* buffer, int n, std::ptrdiff_t byte_stride, int block_size);
  void transpose_inplace(double* buffer, int n, std::ptrdiff_t byte_stride, int block_size);


} // namespace mln::bp


/******************************************/
/****          Implementation          ****/
/******************************************/

namespace mln::bp
{
  namespace impl
  {
    template <class T>
    void transpose_inplace_naive(T* buffer, int n, std::ptrdiff_t byte_stride) noexcept(std::is_nothrow_swappable_v<T>)
    {
      for (int y = 1; y < n; ++y)
      {
        std::byte* row_ptr = (std::byte*)buffer + y * byte_stride;
        std::byte* col_ptr = (std::byte*)buffer + y * sizeof(T);
        for (int x = 0; x < y; ++x)
          std::swap(reinterpret_cast<T*>(row_ptr)[x], *reinterpret_cast<T*>(col_ptr + x * byte_stride));
      }
    }

    // \brief (same as bebore but at different location <=> transpose_swap (location should not overlap)
    // \param (width, height) Dimensions of the input buffer (output buffer is supposed to heigth x width)
    template <class T>
    void transpose_swap_naive(T* __restrict in, T* __restrict out, int width, int height, //
                              std::ptrdiff_t in_byte_stride,
                              std::ptrdiff_t out_byte_stride) noexcept(std::is_nothrow_swappable_v<T>)
    {
      for (int y = 0; y < height; ++y)
      {
        T* row_ptr = ptr_offset(in, y * in_byte_stride);
        T* col_ptr = out + y;

        for (int x = 0; x < width; ++x)
          std::swap(*ptr_offset(col_ptr, x * out_byte_stride), row_ptr[x]);
      }
    }

    // \brief Copy and transpose an input buffer in an output buffer
    // \param (width, height) Dimensions of the **output** buffer (input buffer is supposed to heigth x width)
    template <class T>
    inline void transpose_naive(const T* __restrict src, T* __restrict dst, //
                                int width, int height,                      //
                                std::ptrdiff_t src_stride,
                                std::ptrdiff_t dst_stride) noexcept(std::is_nothrow_swappable_v<T>)
    {
      for (int y = 0; y < height; ++y)
      {
        T*       row_ptr = mln::bp::ptr_offset(dst, y * dst_stride);
        const T* col_ptr = src + y;

        for (int x = 0; x < width; ++x)
          row_ptr[x] = *mln::bp::ptr_offset(col_ptr, x * src_stride);
      }
    }


  } // namespace impl


  template <class T>
  void transpose_inplace(T* buffer, int n, std::ptrdiff_t byte_stride) noexcept(std::is_nothrow_swappable_v<T>)
  {
    impl::transpose_inplace_naive(buffer, n, byte_stride);
  }

  void transpose_inplace(uint8_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(int8_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(uint16_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(int16_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(uint32_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(int32_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(uint64_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(int64_t* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(float* buffer, int n, std::ptrdiff_t stride) noexcept;
  void transpose_inplace(double* buffer, int n, std::ptrdiff_t stride) noexcept;


  template <class T>
  requires(std::copyable<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)) //
      void transpose_inplace(T* buffer, int n, std::ptrdiff_t byte_stride) noexcept
  {
    switch (sizeof(T))
    {
    case 1:
      transpose_inplace((uint8_t*)buffer, n, byte_stride);
      break;
    case 2:
      transpose_inplace((uint16_t*)buffer, n, byte_stride);
      break;
    case 4:
      transpose_inplace((uint32_t*)buffer, n, byte_stride);
      break;
    case 8:
      transpose_inplace((uint64_t*)buffer, n, byte_stride);
      break;
    }
  }


  template <class T>
  [[gnu::noinline]] void transpose(const T* __restrict src, T* __restrict dst, int w, int h, //
                                   std::ptrdiff_t src_stride,
                                   std::ptrdiff_t dst_stride) noexcept(std::is_nothrow_swappable_v<T>)
  {
    impl::transpose_naive(src, dst, w, h, src_stride, dst_stride);
  }


  void transpose(const uint8_t* src, uint8_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const int8_t* src, int8_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const uint16_t* src, uint16_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const int16_t* src, int16_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const uint32_t* src, uint32_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const int32_t* src, int32_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const uint64_t* src, uint64_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const int64_t* src, int64_t* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const float* src, float* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;
  void transpose(const double* src, double* dst, int w, int h, std::ptrdiff_t src_stride,
                 std::ptrdiff_t dst_stride) noexcept;


  template <class T>
  requires(std::copyable<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)) //
      void transpose(const T* __restrict src, T* __restrict dst, int width, int height,                //
                     std::ptrdiff_t src_stride, std::ptrdiff_t dst_stride) noexcept
  {
    switch (sizeof(T))
    {
    case 1:
      transpose((uint8_t*)src, (uint8_t*)dst, width, height, src_stride, dst_stride);
      break;
    case 2:
      transpose((uint16_t*)src, (uint16_t*)dst, width, height, src_stride, dst_stride);
      break;
    case 4:
      transpose((uint32_t*)src, (uint32_t*)dst, width, height, src_stride, dst_stride);
      break;
    case 8:
      transpose((uint64_t*)src, (uint64_t*)dst, width, height, src_stride, dst_stride);
      break;
    }
  }
} // namespace mln::bp
