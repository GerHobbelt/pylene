#pragma once
#include <cstddef>
#include <algorithm>

#include <mln/bp/tile.hpp>
#include <mln/bp/utils.hpp>

namespace mln::bp
{

  /// \brief Copy a 2D-buffer into another
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

  /// \brief Copy a 2D-tile into another
  ///
  /// \rst
  /// :param src: Source tile
  /// :param dst: Destination tile
  ///
  /// .. note: Source has to equal (or included) in the destination
  /// \endrst
  template <typename T>
  void copy(Tile2DView<T> src, Tile2DView<T> dst);


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

  template <class T>
  inline void copy(const T* __restrict src, T* __restrict dst, int w, int h, std::ptrdiff_t src_stride,
                   std::ptrdiff_t dst_stride)
  {
    for (int y = 0; y < h; ++y)
      std::copy_n(mln::bp::ptr_offset(src, y * src_stride), w, mln::bp::ptr_offset(dst, y * dst_stride));
  }


  template <int WIDTH, int HEIGHT, typename T>
  inline void copy(Tile2DView<T> src, Tile2DView<T> dst)
  {
    assert(src.width() <= dst.width());
    assert(src.height() <= dst.height());
    copy<WIDTH, HEIGHT>(src.data(), dst.data(), src.width(), src.height(), src.stride(), dst.stride());
  }

  template <typename T>
  inline void copy(Tile2DView<T> src, Tile2DView<T> dst)
  {
    assert(src.width() <= dst.width());
    assert(src.height() <= dst.height());
    copy(src.data(), dst.data(), src.width(), src.height(), src.stride(), dst.stride());
  }



}
