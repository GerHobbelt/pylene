#pragma once
#include <cstddef>
#include <algorithm>

#include <mln/bp/tile.hpp>
#include <mln/bp/utils.hpp>

namespace mln::bp
{

  /// \brief Swap 2 2D-buffer
  ///
  /// \rst
  /// :param buffer: Buffer source
  /// :param dst: Buffer destination
  /// :param width: Width of the 2D-buffers
  /// :param height: Height of the 2D-buffers
  /// :param src_stride: Stride of the source buffer (in bytes)
  /// :param dst_stride: Stride of the destination buffer (in bytes)
  /// \endrst
  template <typename T>
  inline void fill(Tile2DView<T> buffer, T value);


  template <int WIDTH, int HEIGHT, typename T>
  inline void fill(Tile2DView<T> buffer, T value);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <int WIDTH, int HEIGHT, typename T>
  inline void fill(Tile2DView<T> buffer, T value)
  {
    mln_assume(buffer.width() <= WIDTH);
    mln_assume(buffer.height() <= HEIGHT);

    for (int y = 0; y < buffer.height(); ++y)
      std::fill_n(buffer.row(y), buffer.width(), value);
  }

  template <typename T>
  inline void fill(Tile2DView<T> buffer, T value)
  {
    for (int y = 0; y < buffer.height(); ++y)
      std::fill_n(buffer.row(y), buffer.width(), value);
  }


}
