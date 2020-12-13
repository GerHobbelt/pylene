#include <mln/bp/tile.hpp>

#include <xsimd/xsimd.hpp>


namespace
{


  void vlabelize2d_16x16_pass_1(mln::bp::Tile2DView<bool> tile, mln::bp::Tile2DView<uint8_t> labels)
  {
    constexpr int H = 16;
    constexpr int W = 16;

    using simd_t = xsimd::batch<uint8_t, W>;

    simd_t Xold {uint8_t(0)};
    simd_t L = {
        16 * 0, 16 * 1, 16 * 2,  16 * 3,  16 * 4,  16 * 5,  16 * 6,  16 * 7,
        16 * 8, 16 * 9, 16 * 10, 16 * 11, 16 * 12, 16 * 13, 16 * 14, 16 * 15,
    };

    simd_t X;
    for (int y = 0; y < H; ++y)
    {
      X.load_aligned(reinterpret_cast<uint8_t*>(tile.row(y)));

      L += xsimd::bitwise_andnot(Xold, X); // L[i] = L[i] + 1 if (X[i-1] == 0 and X[i] == 1) else L[i]

      Xold = X;
      X = X * L;
      X.store_aligned(static_cast<uint8_t*>(labels.row(y)));
    }
  }

  template <int W>
  void find(uint8_t* equiv_table, xsimd::batch<uint8_t, W> x)
  {
    while (
    for (int i = 0; i < W
  }

  void vlabelize2d_16x16_pass_2(mln::bp::Tile2DView<uint8_t> labels, uint8_t* __restrict__ equiv_table)
  {
    constexpr int H = 16;
    constexpr int W = 16;

    using simd_t = xsimd::batch<uint8_t, W>;

    simd_t Xold {uint8_t(0)};
    simd_t L    {uint8_t(0)};
    for (int y = 0; y < H; ++y)
    {
      X.load_aligned(reinterpret_cast<uint8_t*>(labels.row(y)));


      L += xsimd::bitwise_andnot(Xold, X); // X[i] = X[i-1] if (X[i-1] == 1 and X[i] == 1) else L[i]

      Xold = X;
      X = X * L;
      X.store_aligned(static_cast<uint8_t*>(labels.row(y)));
    }
  }

}



namespace mln::labeling::details
{

  void labelize_block_16x16(mln::bp::Tile2DView<bool> tile, mln::bp::Tile2DView<uint8_t> labels)
  {
    vlabelize2d_16x16(tile, labels);
  }


}

