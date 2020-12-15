#include <mln/bp/tile.hpp>

#include <xsimd/xsimd.hpp>
#include <mln/core/algorithm/for_each.hpp>

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

  template <class T, int W, class U, std::size_t... Ints>
  void set_from_array(xsimd::batch<T, W>& x, U v[W], std::index_sequence<Ints...>)
  {
    x = xsimd::batch<T, W>(v[Ints]...);
  }

  template <int W>
  void find(uint8_t* equiv_table, xsimd::batch<uint8_t, W>& x)
  {
    int X[W];
    for (int i = 0; i < W; ++i)
    {
      int y = x[i];
      while (y != equiv_table[y])
        y = equiv_table[y];
      X[i] = y;
    }
    set_from_array(x, X, std::make_index_sequence<W>());
  }

  template <class U>
  int FindRoot(U* T, int x)
  {
    while (T[x] != x)
      x = T[x];
    return x;
  }

  // Union with new root in root(A)
  template <class U>
  int Union(U* T, int a, int b)
  {
    int ra = FindRoot(T, a);
    int rb = FindRoot(T, b);
    assert(ra <= rb);
    T[rb] = ra;
    return ra;
  }


  // Reduce the equivalence table
  // n is the number of labels
  template <class U>
  int Reduce(U* T, std::size_t n)
  {
    int k = 0;
    for (std::size_t i = 1; i <= n; ++i)
      if (T[i] == i)
        T[i] = ++k;
      else
        T[i] = T[T[i]];
    return k;
  }


  int labelize_block_16x16_DT_pass_1(mln::bp::Tile2DView<bool>    tile,   //
                                     mln::bp::Tile2DView<uint8_t> labels, //
                                     uint8_t* __restrict__ T, int connection)
  {


    constexpr int H = 16;
    constexpr int W = 16;
    assert(tile.width() == 16);
    assert(tile.height() == 16);
    assert(labels.width() == 16);
    assert(labels.height() == 16);


    uint8_t  L_buf[W + 2] = {0,};
    uint8_t* prev   = L_buf + 1;
    int      nlabel = 0;

    for (int y = 0; y < H; ++y)
    {
      bool*    ty = tile.row(y);
      uint8_t* L  = labels.row(y);


      uint8_t a;
      uint8_t b = prev[-1];
      uint8_t c = prev[0];
      uint8_t d;
      uint8_t e = 0;

      for (int x = 0; x < W; ++x)
      {
        a = b;
        b = c;
        c = prev[x+1];
        d = e;

        if (!ty[x])
        {
          e = 0;
        }
        else if (connection == 1)
        {
          int pattern = ((bool)(a) << 3) + ((bool)(b) << 2) + ((bool)(c) << 1) + (bool)(d);
          // clang-format: off
          switch (pattern)
          {
          case 0b0000: e = ++nlabel; break; // new label
          case 0b0001: e = d; break;
          case 0b1000:
          case 0b1001: e = a; break;
          case 0b1010:
          case 0b1011: e = Union(T,a,c); break;
          case 0b0010: e = c; break;
          case 0b0011: e = Union(T,c,d); break;
          default:     e = b;
          }
        }
        else if (connection == 0)
        {
          int pattern = ((bool)(b) << 1) + (bool)(d);
          switch (pattern)
          {
          case 0b00: e = ++nlabel; break; // new label
          case 0b01: e = d; break;
          case 0b10: e = b; break;
          default:   e = Union(T, b, d); break;
          }
        }
        // clang-format: on
        L[x] = e;
      }
      std::memcpy(prev, L, W * sizeof(uint8_t));
    }
    return nlabel;
  }

  void labelize_block_16x16_DT_pass_2(mln::bp::Tile2DView<uint8_t> labels, //
                                      uint8_t* __restrict__ T)
  {
    for (int y = 0; y < labels.height(); ++y)
    {
      uint8_t* L = labels.row(y);
      for (int x = 0; x < labels.width(); ++x)
        L[x] = T[L[x]];
    }
  }
}



namespace mln::labeling::details
{

  void labelize_block_16x16(mln::bp::Tile2DView<bool> tile, mln::bp::Tile2DView<uint8_t> labels)
  {
    //vlabelize2d_16x16(tile, labels);
    uint8_t T[256];
    for (int i = 0; i < 256; ++i)
      T[i] = i;
    int nlabel = labelize_block_16x16_DT_pass_1(tile, labels, T, 0);
    nlabel = Reduce(T, nlabel);
    labelize_block_16x16_DT_pass_2(labels, T);
    return;
  }


  void labelize_block_16x16_v2(mln::bp::Tile2DView<bool> tile, mln::bp::Tile2DView<uint8_t> labels)
  {
    vlabelize2d_16x16_pass_1(tile, labels);
  }

}

