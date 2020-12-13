#pragma once

#include <mln/bp/tile.hpp>

namespace mln::labeling::details
{

  void labelize_block_16x16(mln::bp::Tile2DView<bool> tile, mln::bp::Tile2DView<uint8_t> labels);


}
