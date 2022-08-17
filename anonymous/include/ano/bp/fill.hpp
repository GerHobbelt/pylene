#pragma once
#include <algorithm>
#include <cstddef>

#include <ano/bp/tile.hpp>
#include <ano/bp/utils.hpp>

namespace ano::bp
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
    ano_assume(buffer.width() <= WIDTH);
    ano_assume(buffer.height() <= HEIGHT);

    for (int y = 0; y < buffer.height(); ++y)
      std::fill_n(buffer.row(y), buffer.width(), value);
  }

  template <typename T>
  inline void fill(Tile2DView<T> buffer, T value)
  {
    for (int y = 0; y < buffer.height(); ++y)
      std::fill_n(buffer.row(y), buffer.width(), value);
  }


} // namespace ano::bp
