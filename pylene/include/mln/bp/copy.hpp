#pragma once
#include <cstddef>
#include <algorithm>

#include <mln/bp/utils.hpp>

namespace mln::bp
{

  /// \brief Swap 2 2D-buffer
  ///
  /// \rst
  /// :param src: Buffer source
  /// :param dst: Buffer destination
  /// :param width: Width of the 2D-buffers
  /// :param height: Height of the 2D-buffers
  /// :param src_stride: Stride of the source buffer (in bytes)
  /// :param dst_stride: Stride of the destination buffer (in bytes)
  /// \endrst
  template <typename T>
  void copy(const T* __restrict src, T* dst, int width, int height, std::ptrdiff_t src_stride, std::ptrdiff_t dst_stride);

  template <int WIDTH, int HEIGHT, typename T>
  void copy(const T* src, T* dst, int width, int height, std::ptrdiff_t src_stride, std::ptrdiff_t dst_stride);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <int WIDTH, int HEIGHT, typename T>
  inline void copy(const T* __restrict src, T* __restrict dst, int w, int h, std::ptrdiff_t src_stride,
                   std::ptrdiff_t dst_stride)
  {
    mln_assume(w <= WIDTH);
    mln_assume(h <= HEIGHT);

    for (int y = 0; y < h; ++y)
      std::copy_n(mln::bp::ptr_offset(src, y * src_stride), w, mln::bp::ptr_offset(dst, y * dst_stride));
  }

}
